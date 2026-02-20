// Fill out your copyright notice in the Description page of Project Settings.

 
#include "GrabberComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"
//Не забываем включит интерфейс
#include "Interfaces/GrabbableInterface.h"
#include "Interfaces/UsableInterface.h"
#include "Items/ABaseThowableItem.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values for this component's properties
UGrabberComponent::UGrabberComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabberComponent::BeginPlay()
{
	Super::BeginPlay();
	SetupPhysicsHandle();
}


// Called every frame
void UGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//Проверяем держим ли предмет, и записываем в переменные его позицию каждый кадр, чтобы держать предмет перед собый
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		FVector PlayerViewLocation;
		FRotator PlayerViewRotation;
	
		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);
		
		FVector TargetLocation = PlayerViewLocation + PlayerViewRotation.Vector() * HoldDistance;
		FRotator TargetRotation = GetComponentRotation();
		
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, PlayerViewRotation);
	}
}
//Пытаемся схватить объект. Обычно по кнопке
void UGrabberComponent::AttemptGrab()
{
	if (!PhysicsHandle) {return;}
	//Если уже был предмет, то отпускаем всесто того, чтобы схватить
	if (HeldComponent)
	{
		ReleaseHeldObject();
		return;
	}
	
	//Записывем данные о столкновении в переменные, чтобы использовать дальше
	FHitResult HitResult = GetPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();
	
	if (ActorHit && ComponentToGrab)
	{
		bool bHasInterface = ActorHit->Implements<UGrabbableInterface>();
		bool bSimulatesPhys = ComponentToGrab->IsSimulatingPhysics();
		{
			if (bHasInterface || bSimulatesPhys)
			{
				if (bHasInterface)
				{
					IGrabbableInterface::Execute_OnGrabbed(ActorHit, this);
				}
				
				AABaseThowableItem* ThrowableItem = Cast<AABaseThowableItem>(ActorHit);
				bool bUsePhysics = true;

				if (ThrowableItem)
				{
					bUsePhysics = ThrowableItem->UsePhysicsHold();
				}
				
				if (bUsePhysics)
				{
					if (ComponentToGrab->IsSimulatingPhysics())
					{
						PhysicsHandle->GrabComponentAtLocationWithRotation(
						ComponentToGrab,
						NAME_None,
						ComponentToGrab->GetComponentLocation(),
						ComponentToGrab->GetComponentRotation()
						);
						HeldComponent = ComponentToGrab;
					}
				}
				else
				{
					// --- ВАРИАНТ Б: ПУШКА (Attach) ---
				
					// Гарантированно выключаем физику
					ComponentToGrab->SetSimulatePhysics(false);
					ComponentToGrab->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
				
					// Прикрепляем к компоненту Grabber (this)
					ActorHit->AttachToComponent(
						this,
						FAttachmentTransformRules::SnapToTargetNotIncludingScale
					);
				
					// Сбрасываем координаты в ноль относительно руки
					ActorHit->SetActorRelativeLocation(FVector::ZeroVector);
					ActorHit->SetActorRelativeRotation(FRotator::ZeroRotator);

					// --- ИСПРАВЛЕНИЕ: ВОТ ЭТОЙ СТРОЧКИ НЕ БЫЛО ---
					HeldComponent = ComponentToGrab; 
					// Теперь система знает, что руки заняты.
				}
			}
		}
	}
}

void UGrabberComponent::ThrowHeldObject()
{
	if (!PhysicsHandle || !HeldComponent) {return;}
	
	//Записываем в переменную объект, который держим прямо сейчас
	AActor* HeldActor = HeldComponent->GetOwner();
	
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);
	
	UPrimitiveComponent* ComponentToThrow = HeldComponent;
	//Не забываем отпустить предмет перед тем, как дать ему импульс
	ReleaseHeldObject();
	
	if (HeldActor && HeldActor->Implements<UGrabbableInterface>())
	{
		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);
		IGrabbableInterface::Execute_OnThrown(HeldActor, PlayerViewRotation.Vector(), ThrowForce);
	}
	else
	{
		FVector ThrowDirection = PlayerViewRotation.Vector();
		ComponentToThrow->AddImpulse(ThrowDirection * ThrowForce, NAME_None, false);
	}
}

void UGrabberComponent::ReleaseHeldObject()
{
	
	if (!PhysicsHandle || !HeldComponent) {return;}
	AActor* HeldActor = HeldComponent->GetOwner();
	
	PhysicsHandle->ReleaseComponent();
	
	if (HeldComponent && HeldActor->Implements<UGrabbableInterface>())
	{
		IGrabbableInterface::Execute_OnReleased(HeldActor, GetOwner());
	}
	HeldComponent = nullptr;
}

void UGrabberComponent::PrimaryAction()
{
	if (!HeldComponent) {return;}
	AActor* HeldActor = HeldComponent->GetOwner();
	if (HeldActor->Implements<UUsableInterface>())
	{
		bool bUsageSuccess = IUsableInterface::Execute_OnUsePressed(HeldActor);
		if (bUsageSuccess) {return;}
	}
	ThrowHeldObject();
}

//Создаем PhysicsHandle на персонаже. Без этого другие функции не смогут его использовать
void UGrabberComponent::SetupPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicsHandleComponent missing on %s! "), *GetOwner()->GetName());
	}
}

//Прописываем трейсинг 
FHitResult UGrabberComponent::GetPhysicsBodyInReach() const
{
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);
	
	FVector Start = PlayerViewLocation;
	FVector End = Start + PlayerViewRotation.Vector() * MaxGrabDistance; 
	
	if (bDrawDebugLines)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);
		DrawDebugSphere(GetWorld(), End, GrabRadius, 10,  FColor::Red, false, 2.0f);
	}
	FCollisionQueryParams Params (FName(TEXT("GrabberTrace")), false, GetOwner());
	FHitResult Hit;
	
	GetWorld()->SweepSingleByChannel(
		Hit,
		Start, 
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(GrabRadius),
		Params);
	
	return Hit;
}

