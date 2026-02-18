// Fill out your copyright notice in the Description page of Project Settings.


#include "ABaseThowableItem.h"

#include "DamagableInterface.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

static const FName COLLISION_PHYSICS(TEXT("PhysicsActor"));
static const FName COLLISION_BLOCK_ALL(TEXT("BlockAll"));



// Sets default values
AABaseThowableItem::AABaseThowableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh-> SetNotifyRigidBodyCollision(true);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ItemMesh;
	
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
	
	ProjectileMovement->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AABaseThowableItem::BeginPlay()
{
	Super::BeginPlay();
	SetState(EThrowableState::Loot);
}

// Called every frame

void AABaseThowableItem::OnGrabbed_Implementation(USceneComponent* GrabberComponent) 
{
	SetState(EThrowableState::Held);
}

void AABaseThowableItem::OnReleased_Implementation()
{
	SetState(EThrowableState::Loot);
}

void AABaseThowableItem::OnThrown_Implementation(FVector Direction, float Magnitude)
{
	SetState(EThrowableState::Thrown);
	
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = Direction * Magnitude;
	}
}

void AABaseThowableItem::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->OnProjectileBounce.RemoveDynamic(this, &AABaseThowableItem::OnProjectileBounce);
	}
	HandleImpact(ImpactResult);
}

void AABaseThowableItem::OnProjectileStop(const FHitResult& ImpactResult)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->OnProjectileStop.RemoveDynamic(this, &AABaseThowableItem::OnProjectileStop);
	}
	HandleImpact(ImpactResult);
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
		break;
	case EThrowableState::Thrown:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(true);
		
		ItemMesh->SetCollisionProfileName(TEXT("BlockAll"));
		ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		
		ProjectileMovement->SetUpdatedComponent(ItemMesh);
		ProjectileMovement->Velocity = FVector::ZeroVector;
		ProjectileMovement->Activate();
		
		ProjectileMovement->OnProjectileBounce.RemoveDynamic(this, &AABaseThowableItem::OnProjectileBounce);
		ProjectileMovement->OnProjectileStop.RemoveDynamic(this, &AABaseThowableItem::OnProjectileStop);
		
		ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AABaseThowableItem::OnProjectileBounce);
		ProjectileMovement->OnProjectileStop.AddDynamic(this, &AABaseThowableItem::OnProjectileStop);
		
		break;
	case EThrowableState::Impact:
		ProjectileMovement->Deactivate();
		SetState(EThrowableState::Loot);
		break;
	}
}

void AABaseThowableItem::HandleImpact_Implementation(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	if (!HitActor) {return;}
	
	if (HitActor->Implements<UDamagableInterface>())
	{
		IDamagableInterface::Execute_TakeDamage(HitActor, this->DamageTags);
	}
	SetState(EThrowableState::Loot);
	
}

