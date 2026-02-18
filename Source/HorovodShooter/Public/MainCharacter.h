#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "DamagableInterface.h"
#include "GameplayTagContainer.h"
#include "MainCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UGrabberComponent;


UCLASS()
class HOROVODSHOOTER_API AMainCharacter : public ACharacter, public IDamagableInterface
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

	// Функция полного сброса состояния (вызывается Контроллером при респауне)
	void ResetCharacterState();

public:	
	// Событие для Блюпринтов (Звуки, Партиклы смерти)
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnPlayerDied();

	// --- COMPONENTS ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGrabberComponent> GrabberComponent;

	// --- INPUT ACTIONS ---
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

	// --- DASH SETTINGS ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementFeel|Dash", meta = (ClampMin = "0.0"))
	float DashForce = 4000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementFeel|Dash", meta = (ClampMin = "0.0"))
	float DashDuration = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementFeel|Dash", meta = (ClampMin = "0.0"))
	float DashCooldown = 1.0f;
	UPROPERTY(BlueprintReadWrite, Category = "MovementFeel|Dash")
	bool bCanDash = true;
	
	
	//TimeDilation
	void ToggleTimeDilation(const FInputActionValue& Value);
	UPROPERTY()
	bool bIsTimeDilated = false;
	
	//Настройки качания головой
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementFeel|Audio")
	float StepDistance = 350.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementFeel|Audio")
	TObjectPtr<USoundBase> FootstepSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementFeel|Audio")
	TObjectPtr<USoundBase> LandSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementFeel|Audio")
	TObjectPtr<USoundBase> DashSound;;
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
	// Таймеры Дэша
	FTimerHandle DashTimerHandle;     // Кулдаун
	FTimerHandle DashDurationTimer;   // Время скольжения

	// Переменные для хранения дефолтного трения
	float DefaultGroundFriction;
	float DefaultBrakingDeceleration;

	void ProcessMovementEffects(float DeltaTime);
	
	
	// --- INPUT HANDLERS ---
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);
	
	// Вспомогательные функции Дэша
	void StopDashing();
	void ResetDash();
	void OnPrimaryAction();
	
	virtual void Landed(const FHitResult& Hit) override;
};