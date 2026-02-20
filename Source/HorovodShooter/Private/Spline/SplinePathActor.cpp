// Fill out your copyright notice in the Description page of Project Settings.


#include "Spline/SplinePathActor.h"

// Sets default values
ASplinePathActor::ASplinePathActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	RootComponent = PathSpline;
	
	PathSpline->EditorUnselectedSplineSegmentColor = FLinearColor(0.5f, 0.0f, 1.0f);
}

// Called when the game starts or when spawned
void ASplinePathActor::BeginPlay()
{
	Super::BeginPlay();	
	
}

// Called every frame
void ASplinePathActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

