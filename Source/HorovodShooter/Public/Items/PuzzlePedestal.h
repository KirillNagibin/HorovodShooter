// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "PuzzlePedestal.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USoundBase;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPedestalActivatedSignature, APuzzlePedestal*, ActivatedPedestal);

USTRUCT(BlueprintType)
struct FPedestalSlot
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Settings")
	FGameplayTag RequiredTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Settings")
	FName AttachSocketName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slot State")
	bool bIsFilled = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slot State")
	TObjectPtr<AActor> AttachedItem = nullptr;
};

UCLASS()
class HOROVODSHOOTER_API APuzzlePedestal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APuzzlePedestal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PedestalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> OverlapZone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle Settings")
	TArray<FPedestalSlot> RequiredSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> ItemInsertedSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> PedestalActivatedSound;
	
	UPROPERTY(BlueprintAssignable, Category = "Puzzle events")
	FOnPedestalActivatedSignature OnPedestalActivated;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle events")
	void OnSlotFilled(int32 SlotIndex, AActor* InsertedItem);
	
	
private:
	UFUNCTION()
	void OnZoneBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);

	bool CheckIfAllSlotsFilled() const;
	void ActivatePedestal();
	
	void ForceReleaseItem(AActor* OtherActor);
	void DisableItemPhysics(AActor* OtherActor);
	void AttachItemToSlot(AActor* OtherActor, FPedestalSlot& Slot, int32 i);
	
	bool bIsActivated = false;


};
