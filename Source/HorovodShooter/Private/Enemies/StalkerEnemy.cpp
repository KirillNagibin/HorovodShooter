// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/StalkerEnemy.h"
#include "Components/DashComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemies/EnemyAIController.h"
#include "TimerManager.h"
#include "AIController.h"
#include "BrainComponent.h"


// Sets default values
AStalkerEnemy::AStalkerEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DashComponent = CreateDefaultSubobject<UDashComponent>(TEXT("DashComponent"));

	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bRequestedMoveUseAcceleration = true;
	}
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI =	EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AStalkerEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
	TargetLookLocation = GetActorLocation() + GetActorForwardVector() * 1000.f;
	
	if (DashComponent)
	{
		DashComponent->OnDashPerformed.AddDynamic(this, &AStalkerEnemy::HandleDashPerformed);
	}
}

// Called every frame
void AStalkerEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CachedPlayer)
	{
		bool bIsMoving = GetVelocity().Size2D() >= 10.f;
		FVector PlayerLocation = CachedPlayer->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
		TargetLookLocation = FMath::VInterpTo(TargetLookLocation, PlayerLocation, DeltaTime, HeadTrackingSpeed);
		
		FVector DirectionToPlayer = (PlayerLocation - GetActorLocation()).GetSafeNormal();
		FRotator TargetRotation = DirectionToPlayer.Rotation();
		FRotator CurrentRotation = GetActorRotation();
		
		float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw);
		
			if (bIsMoving)
			{
				bIsTurning = false;
				GetCharacterMovement()->bUseControllerDesiredRotation = true;
				SpineYaw = FMath::FInterpTo(SpineYaw, 0.0f, DeltaTime, 5.0f);
				TurnRate = 0.0f;
			}
			else
			{
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
			
				if (FMath::Abs(DeltaYaw) > 80.0f && !bIsTurning)
				{
					bIsTurning = true;
				}
				if (bIsTurning && FMath::Abs(DeltaYaw) < 5.0f)
				{
					bIsTurning = false;
				}
				if (bIsTurning)
				{
					FRotator NewRotation = FMath::RInterpConstantTo(CurrentRotation, FRotator(0.0f, TargetRotation.Yaw, 0.0f), DeltaTime, BodyTurnRate * 10.0f);
					SetActorRotation(NewRotation);
					TurnRate = DeltaYaw;
				}
				else
				{
					TurnRate = FMath::FInterpTo(TurnRate, 0.0f, DeltaTime, 10.0f);
				}
			}
		
	}
}


//Enemy plays anymation when takes damage
void AStalkerEnemy::TakeDamage_Implementation(const FGameplayTagContainer& IncomingDamageTags)
{
	if (IncomingDamageTags.HasTag(FGameplayTag::RequestGameplayTag("Damage.Ozempic")))
	{
		if (BlindingMontage)
		{	
			PlayAnimMontage(BlindingMontage);
		}
	}
}



void AStalkerEnemy::OnWarningRecieved_Implementation(FVector HazardLocation, FVector HazardVelocity)
{
	if (!DashComponent) return;
	if (bIsEvading) return;
	
	
	FVector SafeDirection = CalculateEvasionDirection(HazardLocation, HazardVelocity);
	bool bDidDash = DashComponent->PerformDash(SafeDirection);
	
	//Events after dash
	if (bDidDash)
	{
		bIsEvading = true;
		BurstDirection = SafeDirection;
		
		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			AICon->StopMovement();
			if (UBrainComponent* BrainComp = AICon->GetBrainComponent())
			{
				BrainComp->PauseLogic("Evading");
			}
		}
		if (UWorld* World = GetWorld())
		{
			FTimerHandle EvasionTimer;
			World->GetTimerManager().SetTimer(EvasionTimer, this, &AStalkerEnemy::ResetEvasionState, 1.0f, false);
		}
	}
}


void AStalkerEnemy::ResetEvasionState()
{
	bIsEvading = false;
	//Interrupt AI logic if dashing
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* BrainComp = AICon->GetBrainComponent())
		{
			BrainComp->ResumeLogic("Evading");
		}
	}
}

FVector AStalkerEnemy::CalculateEvasionDirection(FVector HazardLocation, FVector HazardVelocity)
{
	FVector HazardDirection = HazardVelocity.GetSafeNormal();
	
	FVector RightRelative = FVector::CrossProduct(HazardDirection, FVector::UpVector).GetSafeNormal();
	FVector LeftRelative = -RightRelative;
	
	FVector ToEnemy = (GetActorLocation() - HazardLocation).GetSafeNormal();
	
	float Dot = FVector::DotProduct(RightRelative, ToEnemy);
	
	FVector PrimaryDirection = (Dot > 0.0f) ? RightRelative : LeftRelative;
	FVector SecondaryDirection = (Dot > 0.0f) ? LeftRelative : RightRelative; 
	
	// Trace to feel where does the hazard come
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bool bPrimaryBlocked = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + PrimaryDirection * EvasionCheckDistance,
		ECC_Visibility,
		Params
		);
	if (!bPrimaryBlocked) return PrimaryDirection;
	
	bool bSecondaryBlocked = GetWorld()->LineTraceSingleByChannel(
	HitResult,
	GetActorLocation(),
	GetActorLocation() + SecondaryDirection * EvasionCheckDistance,
	ECC_Visibility,
	Params
	);
	
	if (!bSecondaryBlocked) return SecondaryDirection;
	
	return HazardDirection;
}

void AStalkerEnemy::HandleDashPerformed(FVector DashDirection)
{
	OnDashEffectsStart(-DashDirection);
}
