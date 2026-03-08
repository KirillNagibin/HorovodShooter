// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/DamagableInterface.h"
#include "Interfaces/WarningRecieverInterface.h"
#include "GameplayTagContainer.h"
#include "StalkerEnemy.generated.h"

class UDashComponent;
class UAnimMontage;

UCLASS()
class HOROVODSHOOTER_API AStalkerEnemy : public ACharacter, public IDamagableInterface, public IWarningRecieverInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStalkerEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void OnWarningRecieved_Implementation(FVector HazardLocation, FVector HazardVelocity) override;
	virtual void TakeDamage_Implementation(const FGameplayTagContainer& IncomingDamageTags) override;

	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDashComponent> DashComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Animation")
	FVector TargetLookLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Animation")
	float TurnRate = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Animation")
	float SpineYaw = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Animation")
	float HeadTrackingSpeed = 5.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Animation")
	float BodyTurnRate = 3.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Animation")
	TObjectPtr<UAnimMontage> BlindingMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Evasion")
	float EvasionCheckDistance = 400.0f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "AI|Evasion")
	bool bIsEvading = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category = "AI|Evasion")
 	FVector BurstDirection;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "AI|Events")
	void OnDashEffectsStart(FVector DashDirection);
	
	UFUNCTION()
	void ResetEvasionState();
	
	
private:
	FVector CalculateEvasionDirection(FVector HazardLocation, FVector HazardVelocity);
	UPROPERTY()
	AActor* CachedPlayer;
	
	UPROPERTY()
	bool bIsTurning;
	
	UFUNCTION()
	void HandleDashPerformed(FVector DashDirection);
};
