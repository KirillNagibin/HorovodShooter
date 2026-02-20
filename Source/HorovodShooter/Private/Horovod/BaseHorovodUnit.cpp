// Fill out your copyright notice in the Description page of Project Settings.


#include "Horovod/BaseHorovodUnit.h"


// Sets default values
ABaseHorovodUnit::ABaseHorovodUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootScene;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	MeshComponent->SetCanEverAffectNavigation(false);

}

// Called when the game starts or when spawned
void ABaseHorovodUnit::BeginPlay()
{
	Super::BeginPlay();
	
	InitialScale = MeshComponent->GetRelativeScale3D();
	InitialRotation = MeshComponent->GetRelativeRotation();
	InitialLocation = MeshComponent->GetRelativeLocation();
	
	if (bRandomizePhase)
	{
		TimeOffset = FMath::RandRange(0, 10);
	}
	StartIdleAnimation();
	
}

void ABaseHorovodUnit::TakeDamage_Implementation(const FGameplayTagContainer& IncomingDamageTags)
{
	if (bIsInvulnerable) {return;}
	
	if (IncomingDamageTags.HasAny(DamageTags))
	{
		HandleDeath();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tags Mismatch. Unit Survived!"));
	}
}

// Called every frame
void ABaseHorovodUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IdleAnimType == EUnitAnimationType::None) {return;}
	
	float Time = GetWorld()->GetTimeSeconds() + TimeOffset;	

	float SineWave = FMath::Sin(Time * AnimSpeed);
	
	
	switch (IdleAnimType)
	{
	case EUnitAnimationType::Pulse:
		{
			float ScaleFactor = 1.0f + (SineWave * AnimAmplitude);
			MeshComponent->SetRelativeScale3D(InitialScale * ScaleFactor);
			break;
		}
	case EUnitAnimationType::Wobble:
		{
			FRotator NewRot = InitialRotation;
			NewRot.Roll += SineWave * AnimAmplitude * 10.f;
			MeshComponent->SetRelativeRotation(NewRot);
			break;
		}
	case EUnitAnimationType::Bobbing:
		{
			FVector NewLoc = InitialLocation;
			NewLoc.Z +=SineWave * AnimAmplitude * 50.0f;
			MeshComponent->SetRelativeLocation(NewLoc);
			break;
		}
		
	case EUnitAnimationType::None:
		{
			break;				
		}
	}
}

void ABaseHorovodUnit::FinishDying()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (AActor* Child : AttachedActors)
	{
		Child->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        
		// Опционально: Если у топора есть физика, включаем её обратно
		if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(Child->GetRootComponent()))
		{
			RootPrim->SetSimulatePhysics(true);
		}
	}
	if (MeshComponent)
	{
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	Destroy();
	
}

void ABaseHorovodUnit::HandleDeath()
{
	if (bIsDying) {return;}
	bIsDying = true;
	
	OnDeathFX();
	
	if (OnUnitDeath.IsBound())
	{
		OnUnitDeath.Broadcast(this);
	}
	
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &ABaseHorovodUnit::FinishDying, DeathDelay);

}

void ABaseHorovodUnit::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
	if (bIsDying) {return;}
	if (!Other) {return;}
	
	if (Other->Implements<UDamagableInterface>())
	{
		//FGameplayTagContainer DeathTags;
		//DeathTags.AddTag(FGameplayTag::RequestGameplayTag("Damage.Contact")); 
		IDamagableInterface::Execute_TakeDamage(Other, DamageTags);
	}
}

