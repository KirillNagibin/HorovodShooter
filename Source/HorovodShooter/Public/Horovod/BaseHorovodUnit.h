// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interfaces/DamagableInterface.h"
#include "GameFramework/Actor.h"
#include "BaseHorovodUnit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDeathSignature, ABaseHorovodUnit*, DiedUnit);

UENUM()
enum class EUnitAnimationType : uint8
{
	None UMETA(DisplayName = "None"),
	Pulse UMETA(DisplayName = "Pulse"),
	Wobble UMETA(DisplayName = "Wobble"),
	Bobbing UMETA(DisplayName = "Bobbing"),
};


UCLASS()
class HOROVODSHOOTER_API ABaseHorovodUnit : public AActor, public IDamagableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseHorovodUnit();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure")
	TObjectPtr<USceneComponent> RootScene;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (Categories = "Damage"))
	FGameplayTagContainer DamageTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	bool bIsInvulnerable = false;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Animation")
	EUnitAnimationType IdleAnimType = EUnitAnimationType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Animation")
	float AnimSpeed = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Animation")
	float AnimAmplitude = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Animation")
	bool bRandomizePhase = true;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnUnitDeathSignature OnUnitDeath;
	
	virtual void TakeDamage_Implementation(const FGameplayTagContainer& IncomingDamageTags) override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector InitialScale;
	FRotator InitialRotation;
	FVector InitialLocation;
	
	float TimeOffset = 0.0f;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float DeathDelay = 0.1f;
	 
	FTimerHandle DeathTimerHandle;
	
	void FinishDying();
	
	bool bIsDying = false;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnDeathFX();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void StartIdleAnimation();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Debug")
	virtual void HandleDeath();
	
	virtual void NotifyHit(
		class UPrimitiveComponent* MyComp, 
		AActor* Other, 
		class UPrimitiveComponent* OtherComp, 
		bool bSelfMoved, 
		FVector HitLocation, 
		FVector HitNormal, 
		FVector NormalImpulse, 
		const FHitResult& Hit) override;
	
};

