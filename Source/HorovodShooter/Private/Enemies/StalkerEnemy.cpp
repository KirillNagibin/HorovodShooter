// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/StalkerEnemy.h"
#include "Components/DashComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

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
	}
}

// Called when the game starts or when spawned
void AStalkerEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
	TargetLookLocation = GetActorLocation() + GetActorForwardVector() * 1000.f;
}

// Called every frame
void AStalkerEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CachedPlayer)
	{
		FVector PlayerLocation = CachedPlayer->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
		TargetLookLocation = FMath::VInterpTo(TargetLookLocation, PlayerLocation, DeltaTime, HeadTrackingSpeed);
	}
}



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
	
	FVector SafeDirection = CalculateEvasionDirection(HazardVelocity);
	
	bool bDidDash = DashComponent->PerformDash(SafeDirection);
	if (bDidDash)
	{
		
	}
}

FVector AStalkerEnemy::CalculateEvasionDirection(FVector HazardVelocity)
{
	FVector HazardDirection = HazardVelocity.GetSafeNormal();
	
	FVector RightEvasion = FVector::CrossProduct(HazardDirection, FVector::UpVector).GetSafeNormal();
	FVector LeftEvasion = -RightEvasion;
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bool bRightBlocked = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + RightEvasion * EvasionCheckDistance,
		ECC_Visibility,
		Params
		);
	if (!bRightBlocked) return RightEvasion;
	
	bool bLeftBlocked = GetWorld()->LineTraceSingleByChannel(
	HitResult,
	GetActorLocation(),
	GetActorLocation() + RightEvasion * EvasionCheckDistance,
	ECC_Visibility,
	Params
	);
	
	if (!bLeftBlocked) return LeftEvasion;
	
	return -HazardDirection;
}