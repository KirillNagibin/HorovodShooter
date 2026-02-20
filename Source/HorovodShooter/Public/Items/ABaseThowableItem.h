// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interfaces/GrabbableInterface.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ABaseThowableItem.generated.h"


UENUM(BlueprintType)
enum class EThrowableState : uint8
{
	Loot UMETA(DisplayName = "Loot"),
	Held UMETA(DisplayName = "Held"),
	Thrown UMETA(DisplayName = "Thrown"),
	Impact UMETA(DisplayName = "Impact")
};
UCLASS()
class HOROVODSHOOTER_API AABaseThowableItem : public AActor, public IGrabbableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABaseThowableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ItemMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EThrowableState CurrentState = EThrowableState::Loot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FGameplayTagContainer DamageTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	bool bUsePhysicsHold = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	bool bCanBeThrownManually = true;
	
	
public:
	virtual void OnGrabbed_Implementation(USceneComponent* GrabberComponent) override;
	virtual void OnReleased_Implementation(AActor* Releaser) override;
	virtual void OnThrown_Implementation(FVector Direction, float Magnitude) override;
	
	//Геттеры
	bool UsePhysicsHold() const { return bUsePhysicsHold; }
	bool CanBeThrownManually() const { return bCanBeThrownManually; }

protected:
	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);
	UFUNCTION(BlueprintCallable)
	void SetState(EThrowableState NewState); 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat", meta = (Categories = "Damage"))
	void HandleImpact(const FHitResult& Hit);
	
};
