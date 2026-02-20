// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimeManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChangedSignature, float, NewGlobalTime);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOROVODSHOOTER_API UTimeManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTimeManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable, Category = "Time Manipulation")
	void ApplyTimeDilation(float NewGlobalTime);

	UPROPERTY(BlueprintReadWrite, Category = "Time Manipulation")
	bool bIsTimeDilation = false;
	
	UPROPERTY(BlueprintAssignable, Category = "TimeManipulation")
	FOnTimeChangedSignature OnTimeChanged;
		
};
