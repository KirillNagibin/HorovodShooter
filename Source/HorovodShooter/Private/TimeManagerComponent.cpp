// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeManagerComponent.h"

#include "GrabberComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTimeManagerComponent::UTimeManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UTimeManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UTimeManagerComponent::ApplyTimeDilation(float NewGlobalTime)
{
	float TargetGlobalTime = 1.f;
	if (!bIsTimeDilation)
	{
		TargetGlobalTime = FMath::Clamp(NewGlobalTime, 0.01f, 1.f);
		bIsTimeDilation = true;
	}
	else
	{
		TargetGlobalTime = 1.f;
		bIsTimeDilation = false;
	}
	UGameplayStatics::SetGlobalTimeDilation(this, TargetGlobalTime);
	OnTimeChanged.Broadcast(TargetGlobalTime);
	
	float CompensationFactor = 1 / TargetGlobalTime;
	AActor* Owner = GetOwner();
	if (Owner)
		{
			Owner->CustomTimeDilation = CompensationFactor;
			TArray<AActor*> AttachedActors;
			Owner->GetAttachedActors(AttachedActors);
			for (AActor* Child : AttachedActors)
			{
				if (Child)
				{
					Child->CustomTimeDilation = CompensationFactor;
				}
			}
			UGrabberComponent* Grabber = Owner->FindComponentByClass<UGrabberComponent>();
			if (Grabber && Grabber->HeldComponent)
			{
				if (AActor* HeldActor = Grabber->HeldComponent->GetOwner())
				{
					HeldActor->CustomTimeDilation = CompensationFactor;
				}
			}
		}
	OnTimeChanged.Broadcast(NewGlobalTime);
}



