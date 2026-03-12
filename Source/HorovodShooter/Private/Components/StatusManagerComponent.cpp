// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StatusManagerComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UStatusManagerComponent::UStatusManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatusManagerComponent::ReceiveStatusEffect(const FGameplayTag& StatusTag, float Duration)
{
	//Validation check
	if (!OwnerCharacter) {return;}
	UWorld* World = GetWorld();
	if (!World) {return;}
	
	//Checking tags
	if (!ActiveStatuses.Contains(StatusTag))
	{
		ActiveStatuses.Add(StatusTag);
		ApplyStatus(StatusTag);
	}
	
	//Create timers and delegates
	FTimerHandle& TimerHandle = ActiveStatuses.FindOrAdd(StatusTag);
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &UStatusManagerComponent::RemoveStatus, StatusTag);
	World->GetTimerManager().SetTimer(TimerHandle, TimerDel, Duration, false);
}


// Called when the game starts
void UStatusManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter && OwnerCharacter->GetCharacterMovement())
	{
		DefaultMaxWalkSpeed = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
	}
}


void UStatusManagerComponent::ClearAllStatuses()
{
	//Validation checks
	UWorld* World = GetWorld();
	if (!World) {return;}
	
	//Clear all statuses one by one
	for (auto& Pair: ActiveStatuses)
	{
		World->GetTimerManager().ClearTimer(Pair.Value);
		if (Pair.Key.MatchesTag(FGameplayTag::RequestGameplayTag("Status.Slowed")) && OwnerCharacter && OwnerCharacter->GetCharacterMovement())
		{
			OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
		}
	}
	ActiveStatuses.Empty();
}


void UStatusManagerComponent::ApplyStatus(const FGameplayTag& StatusTag)
{
	//Validation check
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement()) {return;}
	
	//Go through statuses and apply them
	if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag("Status.Slowed")))
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed * 0.2;
	}
}

void UStatusManagerComponent::RemoveStatus(FGameplayTag StatusTag)
{
	ActiveStatuses.Remove(StatusTag);
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement()) {return;}
	
	//Remove certain tag
	if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag("Status.Slowed")))
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
	}
}

