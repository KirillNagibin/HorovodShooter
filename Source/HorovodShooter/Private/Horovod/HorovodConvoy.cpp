// Fill out your copyright notice in the Description page of Project Settings.


#include "Horovod/HorovodConvoy.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AHorovodConvoy::AHorovodConvoy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}



// Called when the game starts or when spawned
void AHorovodConvoy::BeginPlay()
{
	Super::BeginPlay();
	
	if (TargetPath && TargetPath->PathSpline)
	{
		PathLength = TargetPath->PathSpline->GetSplineLength();
		SpawnWagons();
		
		if (bAutoStart)
		{
			StartConvoy();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SplineFollowerComponent на %s не имеет TargetPath!"), *GetOwner()->GetName());
	}
}

// Called every frame
void AHorovodConvoy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!TargetPath || !TargetPath->PathSpline) {return;}
	
	CurrentDistance += MovementSpeed * DeltaTime;
	
	if (CurrentDistance > PathLength)
	{
		if (bLoopMovement)
		{
			CurrentDistance = FMath::Fmod(CurrentDistance, PathLength);
		}
		else
		{
			CurrentDistance = PathLength;
			StopConvoy();
			OnPathFinished.Broadcast();
		}
	}
UpdateWagonsPosition();
}

void AHorovodConvoy::SpawnWagons()
{
	//Сначала проодим по вагонам на сплайне и удаляем их, если они есть. Заполять потом будем пустой сплайн
	for (AHorovodManager* Wagon : SpawnedWagons)
	{
		if (Wagon) {Wagon->Destroy();}
		SpawnedWagons.Empty();
	}
	
	//Проходим по всем настройкам вагонов, которые раньше записали WagonData
	for (TSoftObjectPtr<UHorovodDataAsset> PresetSoftPtr : WagonPresets)
	{
		UHorovodDataAsset* Preset = PresetSoftPtr.LoadSynchronous();
		if (!Preset) {return;}
		const FHorovodWagonData& Data = Preset->WagonData;
		FTransform SpawnTransform = GetActorTransform();
		//Спавним акторов в отложенном формате. Сначала просто задаем данные и возвращаем указатель
		AHorovodManager* NewWagon = GetWorld()->SpawnActorDeferred<AHorovodManager>(
			AHorovodManager::StaticClass(),
			SpawnTransform,
			this,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
		
		//Завершаем спавн объекта, задавая ему уже готовые данные
		if (NewWagon)
		{
			NewWagon->InitializeFromData(Data);
			NewWagon->SetChaseState(false);
			UGameplayStatics::FinishSpawningActor(NewWagon, SpawnTransform);
			SpawnedWagons.Add(NewWagon);
		}
	}
}

void AHorovodConvoy::UpdateWagonsPosition()
{
	if (!TargetPath || !TargetPath->PathSpline) {return;}
	
	for (int32 i = 0; i < SpawnedWagons.Num(); i++)
	{
		AHorovodManager* Wagon = SpawnedWagons[i];
		if (!Wagon) {continue;}
		
		float WagonDist = CurrentDistance - (i * DistanceBetweenWagons);
		
		if (bLoopMovement)
		{
			if (WagonDist < 0)
			{
				WagonDist += PathLength;
			}
			WagonDist = FMath::Fmod(WagonDist, PathLength);
		}
		else
		{
			WagonDist = FMath::Clamp(WagonDist, 0.0f, PathLength);
		}
		FVector NewLocation = TargetPath->PathSpline->GetLocationAtDistanceAlongSpline(WagonDist, ESplineCoordinateSpace::World);
		FQuat SplineRotation = TargetPath->PathSpline->GetQuaternionAtDistanceAlongSpline(WagonDist, ESplineCoordinateSpace::World);
		
		FQuat NewRotation = SplineRotation * FQuat(RotationOffset);
		Wagon->SetActorLocationAndRotation(NewLocation, NewRotation);
	}
}

void AHorovodConvoy::StartConvoy()
{
	bIsMoving = true;
}

void AHorovodConvoy::StopConvoy()
{
	bIsMoving = false;
}
