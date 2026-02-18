#include "MainCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GrabberComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HorovodPlayerController.h" // Важный инклюд для связи с мозгом
#include "Kismet/GameplayStatics.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true; 
	
	GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCamera->bUsePawnControlRotation = true;
	
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
	GetCharacterMovement()->MaxAcceleration = 600.0f;
	GetCharacterMovement()->AirControl = 0.8f;
	
	// Настраиваем резкую остановку для Doom-like мувмента
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f; 
	GetCharacterMovement()->GroundFriction = 8.0f;
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GrabberComponent = FindComponentByClass<UGrabberComponent>();
	if (!GrabberComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("MainCharacter: GrabberComponent not found! Please add it in Blueprint."));
	}
	
	// Запоминаем нормальное трение для механики Дэша
	DefaultGroundFriction = GetCharacterMovement()->GroundFriction;
	DefaultBrakingDeceleration = GetCharacterMovement()->BrakingDecelerationWalking;
	
	if (FirstPersonCamera)
	{
		DefaultCameraLocation = FirstPersonCamera->GetRelativeLocation();
	}
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bEnableHeadBob)
	{
		ProcessMovementEffects(DeltaTime);
	}
	UE_LOG(LogTemp, Warning, TEXT("%f"), GetVelocity().Z);
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMainCharacter::Dash);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMainCharacter::OnPrimaryAction);
	}
}

void AMainCharacter::TakeDamage_Implementation(const FGameplayTagContainer& IncomingDamageTags)
{
	// 1. Если уже мертвы - игнорируем урон
	if (bIsDead) return;
	
	// 2. Проверяем тэг (используем правильный синтаксис с точкой)
	if (IncomingDamageTags.HasTag(FGameplayTag::RequestGameplayTag("Damage.Contact")))
	{
		bIsDead = true;
		
		UE_LOG(LogTemp, Warning, TEXT("Character: Fatal Damage Received. Calling Controller..."));
		
		// 3. Отключаем физику тела (это ответственность Пешки)
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		
		// 4. Запускаем визуал (Звук/Партиклы в Блюпринте)
		OnPlayerDied();

		// 5. Делегируем логику игры Контроллеру
		if (AHorovodPlayerController* PC = Cast<AHorovodPlayerController>(Controller))
		{
			PC->OnPawnDeath();
		}
	}
}

void AMainCharacter::ResetCharacterState()
{
	bIsDead = false;
	
	// Включаем физику обратно
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	
	// Сбрасываем дэш, если умерли в кд
	ResetDash(); 
	StopDashing(); // Возвращаем трение на место
}

void AMainCharacter::ProcessMovementEffects(float DeltaTime)
{
	if (!bEnableHeadBob || !FirstPersonCamera) {return;}
	if (!bCanDash) return;
	
	float Speed = GetVelocity().Size2D();
	bool bIsMoving = Speed > 10.f && GetCharacterMovement()->IsMovingOnGround();
	
	if (bIsMoving)
	{
		DistanceAccumulator += Speed * DeltaTime;
		float StepPhase = DistanceAccumulator / StepDistance;
		float WaveArgument = StepPhase * UE_TWO_PI;
		if (StepPhase > 0.75f && !bHasPlayedSound)
		{
			if (FootstepSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					FootstepSound,
					GetActorLocation(),
					1.f,
					FMath::RandRange(0.9f, 1.1f)
					);
			}
			DistanceAccumulator -= StepDistance;
			bHasPlayedSound = false;
		}
		float ZOffset = FMath::Sin(WaveArgument) * BobAmplitude; 
		FVector NewLocation = DefaultCameraLocation;
		NewLocation.Z += ZOffset;
		FirstPersonCamera->SetRelativeLocation(NewLocation);
	}
	else
	{
		FVector NewLocation = FMath::VInterpTo(
			FirstPersonCamera->GetRelativeLocation(),
			DefaultCameraLocation,
			DeltaTime,
			10.f
			);
		FirstPersonCamera->SetRelativeLocation(NewLocation);
		DistanceAccumulator = 0.0;
		bHasPlayedSound = false;
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller == nullptr) return;
	
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller == nullptr) return;
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AMainCharacter::Dash(const FInputActionValue& Value)
{
	if (!bCanDash) return;
	
	// Убираем трение для скольжения
	GetCharacterMovement()->GroundFriction = 0.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 0.0f;
	
	FVector DashDirection = GetLastMovementInputVector();
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = GetActorForwardVector();
	}
	
	LaunchCharacter(DashDirection * DashForce, true, true);
	OnDashStart();
	if (DashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DashSound, GetActorLocation());
	}
	bCanDash = false;
	
	if (UWorld* World = GetWorld())
	{
		// Таймер остановки скольжения (Длительность Дэша)
		World->GetTimerManager().SetTimer(DashDurationTimer, this, &AMainCharacter::StopDashing, DashDuration, false);
		
		// Таймер перезарядки (Кулдаун)
		World->GetTimerManager().SetTimer(DashTimerHandle, this, &AMainCharacter::ResetDash, DashCooldown, false);
	}
}

void AMainCharacter::StopDashing()
{
	// Возвращаем нормальное трение
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDeceleration;
}

void AMainCharacter::ResetDash()
{
	bCanDash = true;
}

void AMainCharacter::OnPrimaryAction()
{
	if (GrabberComponent)
	{
		GrabberComponent->PrimaryAction();
	}
}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	if (!LandSound) {return;}
	if (GetVelocity().Z <= -1000.f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LandSound, Hit.Location);
	
		if (LandCameraShake)
		{
			if (APlayerController* PC = Cast<APlayerController>(Controller))
			{
				PC->ClientStartCameraShake(LandCameraShake);
			}
		}
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, Hit.Location);
		DistanceAccumulator = StepDistance * 0.9f; 
		bHasPlayedSound = false;
	}
}

