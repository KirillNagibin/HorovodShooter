#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interfaces/DamagableInterface.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "GameplayTagContainer.h"
#include "MainCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UGrabberComponent;	
class UDashComponent;
class UStatusManagerComponent;


UCLASS()
class HOROVODSHOOTER_API AMainCharacter : public ACharacter, public IDamagableInterface, public IStatusReceiverInterface
{
	GENERATED_BODY()

public:
	AMainCharacter();

protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Интерфейс получения урона
	virtual void TakeDamage_Implementation(const FGameplayTagContainer& IncomingDamageTags) override;
	
	virtual void ReceiveStatusEffect_Implementation(const FGameplayTag& StatusTag, float Duration) override;
	
	// Функция полного сброса состояния (вызывается Контроллером при респауне)
	void ResetCharacterState();

public:	
	// Событие для Блюпринтов (Звуки, Партиклы смерти)
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnPlayerDied();
	UFUNCTION(BlueprintImplementableEvent, Category = "Dash")
	void OnDashStart();

	// COMPONENTS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGrabberComponent> GrabberComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDashComponent> DashComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStatusManagerComponent> StatusManager;

	// INPUT ACTIONS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DashAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleTimeDilationAction;
	

	//Настройки качания головой
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementFeel|Audio")
	float StepDistance = 350.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementFeel|Audio")
	TObjectPtr<USoundBase> FootstepSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementFeel|Audio")
	TObjectPtr<USoundBase> LandSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementFeel|Audio")
	TSubclassOf<UCameraShakeBase> LandCameraShake;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementFeel|HeadBob")
	bool bEnableHeadBob = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementFeel|HeadBob")
	float BobAmplitude = 1.f;
	
	bool bIsDead = false;
private:

	FVector DefaultCameraLocation;
	float DistanceAccumulator = 0.f;
	bool bHasPlayedSound = false;
	
	
protected:
	

	void ProcessMovementEffects(float DeltaTime);
	
	
	// --- INPUT HANDLERS ---
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);
	
	void OnPrimaryAction();
	
	virtual void Landed(const FHitResult& Hit) override;
};