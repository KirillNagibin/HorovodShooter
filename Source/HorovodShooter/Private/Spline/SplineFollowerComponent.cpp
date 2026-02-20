// Fill out your copyright notice in the Description page of Project Settings.


#include "Spline/SplineFollowerComponent.h"

// Sets default values for this component's properties
USplineFollowerComponent::USplineFollowerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bIsMoving = false;
	CurrentDistance = 0.0f;
	PathLength = 0.0f;
}



// Called when the game starts
void USplineFollowerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (TargetPath && TargetPath->PathSpline)
	{
		PathLength = TargetPath->PathSpline->GetSplineLength();
		
		if (bAutoStart)
		{
			StartMoving();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SplineFollowerComponent на %s не имеет TargetPath!"), *GetOwner()->GetName());
	}
}


// Called every frame
void USplineFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsMoving || !TargetPath || !TargetPath->PathSpline) {return;}
	
	CurrentDistance += MovementSpeed * DeltaTime;
	
	if (CurrentDistance >= PathLength)
	{
		if (bLoopMovement)
		{
			CurrentDistance = FMath::Fmod(CurrentDistance, PathLength);
		}
		else
		{
			CurrentDistance = PathLength;
			bIsMoving = false;
			//OnPathFinished.Broadcast();
		}
	}
	
	FVector NewLocation = TargetPath->PathSpline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	
	AActor* Owner = GetOwner();
	if (Owner)
	{
		if (bApplyRotation)
		{
			FQuat SplineRotation = TargetPath->PathSpline->GetQuaternionAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
			FQuat NewRotation = SplineRotation * FQuat(RotationOffset);
			Owner->SetActorLocationAndRotation(NewLocation, NewRotation.Rotator());
		}
		else
		{
			Owner->SetActorLocation(NewLocation);
		}
	}
}

void USplineFollowerComponent::StartMoving()
{
	bIsMoving = true;
}

void USplineFollowerComponent::StopMoving()
{
	bIsMoving = false;
	CurrentDistance = 0.0f;
}

void USplineFollowerComponent::PauseMoving()
{
	bIsMoving = false;
}

