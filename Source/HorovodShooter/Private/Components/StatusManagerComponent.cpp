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
	if (!OwnerCharacter) {return;}
	UWorld* World = GetWorld();
	if (!World) {return;}
	
	if (!ActiveStatuses.Contains(StatusTag))
	{
		ActiveStatuses.Add(StatusTag);
	}
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
	UWorld* World = GetWorld();
	if (!World) {return;}
	
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
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement()) {return;}
	if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag("Status.Slowed")))
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed * 0.2;
	}
}

void UStatusManagerComponent::RemoveStatus(FGameplayTag StatusTag)
{
	ActiveStatuses.Remove(StatusTag);
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement()) {return;}
	if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag("Status.Slowed")))
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
	}
}

