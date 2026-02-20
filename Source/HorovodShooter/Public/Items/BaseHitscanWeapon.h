// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ABaseThowableItem.h"
#include "Interfaces/UsableInterface.h"
#include "BaseHitscanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class HOROVODSHOOTER_API ABaseHitscanWeapon : public AABaseThowableItem, public IUsableInterface
{
	GENERATED_BODY()
	
public:
	ABaseHitscanWeapon();
	
	virtual bool OnUsePressed_Implementation() override;
	virtual void HandleImpact_Implementation(const FHitResult& Hit) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaTime ) override;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> MuzzleLocation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "1"))
	int32 MaxAmmo = 10;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bIsArmedToExplode = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 CurrentAmmo;
	
	UPROPERTY(EditDefaultsOnly)
	float ShotRange = 100;
	
	UPROPERTY(EditDefaultsOnly)
	int32 PiercingCount = 2;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float WeaponLength = 430.l;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float RetractionSpeed = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float RetractionDistance = 400.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float ExplosionRadius = 100.0f;
	
	UFUNCTION(BlueprintCallable)
	void Explode();
	
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
	void OnFire(const FVector& TraceStart, const FVector& TraceEnd, bool bIsLastShot);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
	void OnExplode();
	
	
private:
	float CurrentRetractOffset = 0.0f;

	void ProcessWallAvoidance(float DeltaTime);
	void PerformFire();
	

	
};
