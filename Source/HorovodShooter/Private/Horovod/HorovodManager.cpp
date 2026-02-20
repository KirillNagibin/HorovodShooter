// Fill out your copyright notice in the Description page of Project Settings.


#include "Horovod/HorovodManager.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AHorovodManager::AHorovodManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Создаем сферу-коллайдер как центральный объект
	RootCollider = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollider"));
	RootComponent = RootCollider;
	RootCollider->SetSphereRadius(50.0f);
	RootCollider->SetCollisionProfileName(TEXT("Pawn"));
	RootCollider->SetCanEverAffectNavigation(false);
	//Прикрепляем сплайн к сфере
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));	
	SplineComponent->SetupAttachment(RootCollider);
	SplineComponent->SetClosedLoop(true);
	
	ShapeGenerator = CreateDefaultSubobject<USplineShapeGeneratorComponent>(TEXT("ShapeGenerator"));
}

// Called when the game starts or when spawned
void AHorovodManager::BeginPlay()
{
	Super::BeginPlay();
	if (!bInitializedByConvoy)
	{
		SpawnUnits();
	}
}

// Called every frame
void AHorovodManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (WagonData.bIsChasing)
	{
		ProcessChasing(DeltaTime);
	}
	if (WagonData.bCirculate)
	{
		float DirectionMultiplyer = WagonData.bReverseDirection ? -1.0f : 1.0f;
		CurrentGlobalDistance += WagonData.MovementSpeed * DirectionMultiplyer * DeltaTime;
	}
	UpdateUnitPosition();

}

void AHorovodManager::InitializeFromData(const FHorovodWagonData& NewData)
{
	
	WagonData = NewData;
	bInitializedByConvoy = true;
	
	if (ShapeGenerator)
	{
		ShapeGenerator->SplineShapeSettings = NewData.SplineShapeSettings;
		ShapeGenerator->GenerateSpline(SplineComponent);
	}
	SpawnUnits();
	
}



void AHorovodManager::SetChaseState(bool bEnabled)
{
	WagonData.bIsChasing = bEnabled;
}

void AHorovodManager::RebuildSpline()
{
	if (ShapeGenerator)
	{
		ShapeGenerator->SplineShapeSettings = WagonData.SplineShapeSettings;
		ShapeGenerator->GenerateSpline(SplineComponent);
	}
}

void AHorovodManager::EditorRebuildSpline()
{
#if WITH_EDITOR
	RebuildSpline();
	Modify();
#endif
	
}


void AHorovodManager::SpawnUnits()
{
	if (WagonData.SpawnPattern.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No actors assigned"));
		return;
	}
	
	SplineLength = SplineComponent->GetSplineLength();
	
	if (SplineLength <= 0.0f) {return;}
	UnitGap = SplineLength / (float)WagonData.TotalUnitsCount;
	
	AliveUnits.Empty();
	WagonData.AliveCount = 0;
	
	FVector StartLocation = SplineComponent->GetComponentLocation();
	FRotator StartRotation = SplineComponent->GetComponentRotation();
	
	for (int32 i = 0; i < WagonData.TotalUnitsCount; i++)
	{
		int32 PatternIndex = i % WagonData.SpawnPattern.Num();
		TSubclassOf<ABaseHorovodUnit> ClassToSpawn = WagonData.SpawnPattern[PatternIndex];
		
		if (!ClassToSpawn) {continue;}
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		ABaseHorovodUnit* NewUnit = GetWorld()->SpawnActor<ABaseHorovodUnit>(ClassToSpawn, StartLocation, StartRotation, SpawnParams);
		
		if (NewUnit)
		{
			NewUnit->OnUnitDeath.AddDynamic(this, &AHorovodManager::OnUnitDied);
			AliveUnits.Add(NewUnit);
			WagonData.AliveCount++;
		}
		else 
			AliveUnits.Add(nullptr);
	}
	UpdateUnitPosition();
}

void AHorovodManager::UpdateUnitPosition()
{
	if (SplineLength <= 0.0f) {return;}
	
	for (int32 i = 0; i < AliveUnits.Num(); i++)
	{
		ABaseHorovodUnit* Unit = AliveUnits[i];
		if (!Unit) {continue;}
		
		float BaseDistance = UnitGap * i;
		float FinalDistance = BaseDistance + CurrentGlobalDistance;
		
		float DistanceOnSpline = FMath::Fmod(FinalDistance, SplineLength);
		
		if (DistanceOnSpline < 0.0f)
		{
			DistanceOnSpline += SplineLength;
		}
		
		FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceOnSpline, ESplineCoordinateSpace::World);
		Unit->SetActorLocation(Location);	
		if (WagonData.bRotateAsSpline)
		{
			FRotator Rotation = SplineComponent->GetRotationAtDistanceAlongSpline(DistanceOnSpline, ESplineCoordinateSpace::World);
			Unit->SetActorLocationAndRotation(Location, Rotation);
		}
	}
}

void AHorovodManager::ProcessChasing(float DeltaTime)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) {return;}
	
	FVector TargetLocation = PlayerPawn->GetActorLocation();
	FVector CurrentLocation = GetActorLocation();
	
	if (WagonData.bLockHeight)
	{
		TargetLocation.Z = CurrentLocation.Z;
	}
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, WagonData.ChaseInterpSpeed);
	
	FHitResult Hit;
	SetActorLocation(NewLocation, true, &Hit);
}



void AHorovodManager::OnUnitDied(ABaseHorovodUnit* DeadUnit)
{
	if (!DeadUnit) {return;}
	
	int32 index = AliveUnits.Find(DeadUnit);
	
	if (index != INDEX_NONE)
	{
		AliveUnits[index] = nullptr;
		WagonData.AliveCount--;
		UE_LOG(LogTemp, Warning, TEXT("Unit at index %d died. Alive %d"), index, WagonData.AliveCount);
		if (WagonData.AliveCount <=0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Horovod Defeated!"));
		}
	}
}


