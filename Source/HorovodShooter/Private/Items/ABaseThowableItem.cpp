// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ABaseThowableItem.h"

#include "MainCharacter.h"
#include "TimeManagerComponent.h"
#include "Interfaces/DamagableInterface.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/WarningRecieverInterface.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AABaseThowableItem::AABaseThowableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh-> SetNotifyRigidBodyCollision(true);
	ItemMesh->SetGenerateOverlapEvents(true);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ItemMesh;
	
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
	
	ProjectileMovement->bAutoActivate = false;
}

void AABaseThowableItem::BeginPlay()
{
	Super::BeginPlay();
	SetState(EThrowableState::Loot);
}

void AABaseThowableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bHasSentWarning)
	{
		WarningCooldown -= DeltaTime;
		if (WarningCooldown <= 0.0f)
		{
			bHasSentWarning = false;
			WarningCooldown = 0.0f;
		}
	}
	if (CurrentState == EThrowableState::Thrown)
	{
		CheckTreatedActors(DeltaTime);
	}
}

void AABaseThowableItem::StickOnHit(const FHitResult& Hit)
{	
	if (!Hit.GetComponent()) {return;}
	if (Hit.GetActor() == UGameplayStatics::GetPlayerPawn(this, 0))
	{
		return;
	}	
	if (ProjectileMovement)
	{
		ProjectileMovement->OnProjectileBounce.RemoveDynamic(this, &AABaseThowableItem::OnProjectileBounce);
		ProjectileMovement->OnProjectileStop.RemoveDynamic(this, &AABaseThowableItem::OnProjectileStop);
		
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
		
		ItemMesh->SetSimulatePhysics(false);
		
		AttachToComponent(Hit.GetComponent(), FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
		SetActorTickEnabled(false);
	}
}


void AABaseThowableItem::OnGrabbed_Implementation(USceneComponent* GrabberComponent) 
{
	SetState(EThrowableState::Held);
	if (GrabberComponent && GrabberComponent->GetOwner())
	{
		this->CustomTimeDilation = GrabberComponent->GetOwner()->CustomTimeDilation;
	}
}

void AABaseThowableItem::OnReleased_Implementation(AActor* Releaser)
{
	SetState(EThrowableState::Loot);
}

void AABaseThowableItem::OnThrown_Implementation(FVector Direction, float Magnitude)
{
	SetState(EThrowableState::Thrown);
	this->CustomTimeDilation = 1.0f;
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = Direction * Magnitude;
	}
	
}

void AABaseThowableItem::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	AActor* HitActor = ImpactResult.GetActor();
	if (!HitActor) {return;}
	HandleImpact(ImpactResult);
}

void AABaseThowableItem::OnProjectileStop(const FHitResult& ImpactResult)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->OnProjectileBounce.RemoveDynamic(this, &AABaseThowableItem::OnProjectileBounce);
		ProjectileMovement->OnProjectileStop.RemoveDynamic(this, &AABaseThowableItem::OnProjectileStop);
	}
	HandleImpact(ImpactResult);
	if (!GetRootComponent()->GetAttachParent())
	{
		SetState(EThrowableState::Loot);
	}
}

void AABaseThowableItem::SetState(EThrowableState NewState)
{
	if (CurrentState == NewState && NewState != EThrowableState::Loot) {return;}
	
	CurrentState = NewState;
	switch (CurrentState)
	{
	case EThrowableState::Loot:
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
		ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		ProjectileMovement->Deactivate();
		ProjectileMovement->Velocity = FVector::ZeroVector;
		SetActorTickEnabled(false);
		break;
	case EThrowableState::Held:
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		if (bUsePhysicsHold)
		{
			ItemMesh->SetSimulatePhysics(true);
			ItemMesh->SetEnableGravity(false);
		}
		else
		{
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
		}
		ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		ProjectileMovement->Deactivate();
		SetActorTickEnabled(false);
		break;
	case EThrowableState::Thrown:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(true);
		
		ItemMesh->SetCollisionProfileName(TEXT("BlockAll"));
		//ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		
		ProjectileMovement->SetUpdatedComponent(ItemMesh);
		ProjectileMovement->Velocity = FVector::ZeroVector;
		ProjectileMovement->Activate();
		
		ProjectileMovement->OnProjectileBounce.RemoveDynamic(this, &AABaseThowableItem::OnProjectileBounce);
		ProjectileMovement->OnProjectileStop.RemoveDynamic(this, &AABaseThowableItem::OnProjectileStop);
		
		ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AABaseThowableItem::OnProjectileBounce);
		ProjectileMovement->OnProjectileStop.AddDynamic(this, &AABaseThowableItem::OnProjectileStop);
		
		bHasSentWarning = false;
		WarningCooldown = 0.0f;
		SetActorTickEnabled(true);

		break;
	case EThrowableState::Impact:
		ProjectileMovement->Deactivate();
		SetState(EThrowableState::Loot);
		break;
	}
}


void AABaseThowableItem::HandleImpact_Implementation(const FHitResult& Hit)
{
	if (Hit.GetActor()->Implements<UDamagableInterface>())
	{
		IDamagableInterface::Execute_TakeDamage(Hit.GetActor(), this->DamageTags);
	}
}

void AABaseThowableItem::CheckTreatedActors(float DeltaTime)
{
	if (bHasSentWarning) {return;}
	
	FVector StartLocation = GetActorLocation();
	FVector Velocity = ProjectileMovement ? ProjectileMovement-> Velocity : GetVelocity();
	
	FVector EndLocation = StartLocation + Velocity * WarningLookAheadTime;
	
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(this, 0));
	
	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(WarningSphereRadius),
		CollisionParams
		);
	
	/* DrawDebugCapsule(
		GetWorld(),
		StartLocation + (EndLocation - StartLocation) * 0.5f,
		(EndLocation - StartLocation).Size() * 0.5f, 
		WarningSphereRadius,
		(EndLocation - StartLocation).Rotation().Quaternion(),
		bHit ? FColor::Red : FColor::Green,
		false,
		0.1);
	*/
	
	if (bHit && Hit.GetActor())
	{
		if (Hit.GetActor()->Implements<UWarningRecieverInterface>())
		{
			IWarningRecieverInterface::Execute_OnWarningRecieved(Hit.GetActor(), StartLocation, Velocity);
			bHasSentWarning = true;
			WarningCooldown = 0.5f;
		}
	}
}