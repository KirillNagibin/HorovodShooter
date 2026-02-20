// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Spline/SplineShapeGeneratorComponent.h"
#include "Horovod/BaseHorovodUnit.h"
#include "HorovodManager.generated.h"



USTRUCT(BlueprintType)
struct FHorovodWagonData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horovod Settings")
	int32 TotalUnitsCount = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horovod Settings")
	TArray<TSubclassOf<ABaseHorovodUnit>> SpawnPattern;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horovod Settings")
	float MovementSpeed = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horovod Settings")
	bool bReverseDirection = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Horovod Settings")
	int32 AliveCount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horovod Settings")
	bool bIsChasing = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horovod Settings")
	float ChaseInterpSpeed = 2.0f;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horovod Settings")
	bool bLockHeight = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horovod Settings")
	bool bRotateAsSpline = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horovod Settings")
	bool bCirculate = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horovod Settings")
	FSplineShapeSettings SplineShapeSettings;
	
	
};

UCLASS()
class HOROVODSHOOTER_API AHorovodManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHorovodManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USplineShapeGeneratorComponent* ShapeGenerator;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USplineComponent> SplineComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USphereComponent> RootCollider;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HorovodData", meta = (ShowOnlyInnerProperties))
	FHorovodWagonData WagonData;
	
	UFUNCTION(BlueprintCallable, Category = "HorovodData")
	void InitializeFromData(const FHorovodWagonData& NewData);
	
	UFUNCTION(BlueprintCallable, Category = "Horovod Settings")
	void SetChaseState(bool bEnabled);
	
	UFUNCTION(BlueprintCallable, Category = "Horovod Settings")
	void RebuildSpline();
	
	UFUNCTION(CallInEditor, Category = "Horovod Settings")
	void EditorRebuildSpline();

	
private:
	UPROPERTY()
	TArray<TObjectPtr<ABaseHorovodUnit>> AliveUnits;
	
	float CurrentGlobalDistance = 0.0f;
	float UnitGap = 0.0f;
	float SplineLength = 0.0f;
	
	bool bInitializedByConvoy = false;


	void SpawnUnits();
	void UpdateUnitPosition();
	void ProcessChasing(float DeltaTime);
	
	
	UFUNCTION()
	void OnUnitDied(ABaseHorovodUnit* DeadUnit);
};
