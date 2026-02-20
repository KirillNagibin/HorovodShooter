// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Horovod/HorovodManager.h"
#include "Horovod/HorovodDataAsset.h"
#include "Spline/SplinePathActor.h"
#include "HorovodConvoy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPathFinishedSignature);

UCLASS()
class HOROVODSHOOTER_API AHorovodConvoy : public AActor
{
	GENERATED_BODY()
	
	
public:	
	// Sets default values for this actor's properties
	AHorovodConvoy();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConvoyPath")
	TObjectPtr<ASplinePathActor> TargetPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConvoyPath")
	float MovementSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConvoyPath")
	FRotator RotationOffset = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConvoyPath")
	bool bAutoStart = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConvoyPath")
	bool bLoopMovement = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConvoyPath")
	bool bApplyRotation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConvoyComposition")
	float DistanceBetweenWagons = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConvoyComposition")
	TArray<TSoftObjectPtr<UHorovodDataAsset>> WagonPresets;
	
	
	UPROPERTY(BlueprintAssignable, Category = "ConvoyPath|Events")
	FOnPathFinishedSignature OnPathFinished;
	
	UFUNCTION(BlueprintCallable, Category = "ConvoyControls")
	void StartConvoy();
	UFUNCTION(BlueprintCallable, Category = "ConvoyControls")
	void StopConvoy();

	
private:
	UPROPERTY()
	TArray<AHorovodManager*> SpawnedWagons;
	
	bool bIsMoving;
	float CurrentDistance;
	float PathLength;
	
	void SpawnWagons();
	void UpdateWagonsPosition();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
