// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DashComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"


// Sets default values for this component's properties
UDashComponent::UDashComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;


}


// Called when the game starts
void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Dash isnt attached to a character. It's attached to a %s"), *GetOwner()->GetName());
	}
	
	MovementComponent = OwnerCharacter->GetCharacterMovement();
	OriginalGroundFriction = MovementComponent->GroundFriction;
	OriginalBrakingDeceleration = MovementComponent->BrakingDecelerationWalking;
}



bool UDashComponent::PerformDash(FVector DashDirection)
{
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement()) return false;
	
	UWorld* World = GetWorld();
	if (!World) return false;
	
	float CurrentTime = GetWorld()->GetRealTimeSeconds();
	
	if (CurrentTime - LastDashRealTime < DashCooldown) return false;
	
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = OwnerCharacter->GetActorForwardVector();
	}
	else
	{
		DashDirection.Normalize();
	}
	
	MovementComponent->GroundFriction = 0.0f;
	MovementComponent->BrakingDecelerationWalking = 0.0f;
	
	OwnerCharacter->LaunchCharacter(DashDirection * DashForce, true, true);
	
	if (DashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DashSound, OwnerCharacter->GetActorLocation());
	}
	
	LastDashRealTime = CurrentTime;
	
	float CurrentTimeDilation = UGameplayStatics::GetGlobalTimeDilation(this);
	float AdjustedDuration = CurrentTimeDilation - LastDashRealTime;
	
	World->GetTimerManager().SetTimer(DashDurationTimer, this, &UDashComponent::StopDashing, AdjustedDuration, false);

	return true;
}


void UDashComponent::StopDashing()
{
	if (OwnerCharacter && OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->GroundFriction = OriginalGroundFriction;
		MovementComponent->BrakingDecelerationWalking = OriginalBrakingDeceleration;
	}
}