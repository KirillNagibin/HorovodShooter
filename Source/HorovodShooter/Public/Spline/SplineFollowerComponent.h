// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Spline/SplinePathActor.h"
#include "SplineFollowerComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPathFinishedSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOROVODSHOOTER_API USplineFollowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USplineFollowerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	TObjectPtr<ASplinePathActor> TargetPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	float MovementSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	FRotator RotationOffset = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	bool bAutoStart = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	bool bLoopMovement = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower")
	bool bApplyRotation = false;
	
	UFUNCTION(BlueprintCallable, Category = "SplineFollower|Control")
	void StartMoving();
	UFUNCTION(BlueprintCallable, Category = "SplineFollower|Control")
	void StopMoving();
	UFUNCTION(BlueprintCallable, Category = "SplineFollower|Control")
	void PauseMoving();
	
	//UPROPERTY(BlueprintCallable, Category = "SplineFollower|Events")
	//FOnPathFinishedSignature OnPathFinished;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	bool bIsMoving;
	float CurrentDistance;
	float PathLength;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
