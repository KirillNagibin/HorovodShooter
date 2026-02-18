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
	float ClampedTime = FMath::Clamp(NewGlobalTime, 0.f, 1.f);
	
	UGameplayStatics::SetGlobalTimeDilation(this, ClampedTime);
	
	float CompensationFactor = 1 / ClampedTime;
	
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
}



