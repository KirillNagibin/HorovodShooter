// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOROVODSHOOTER_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDashComponent();

	
	UFUNCTION(BlueprintCallable, Category = "Dash")
	void PerformDash(FVector DashDirection);
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Settings")
	float DashForce = 20000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Settings")
	float DashDuration = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Settings")
	float DashCooldown = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash|Settings")
	TObjectPtr<class USoundBase> DashSound;

	
private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
	
	FTimerHandle DashDurationTimer;
	
	float LastDashRealTime = -100.f;

	float OriginalGroundFriction;
	float OriginalBrakingDeceleration;
	
	void StopDashing();


		
};
