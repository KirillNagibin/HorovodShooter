// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PuzzlePedestal.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Items/ABaseThowableItem.h"
#include "GrabberComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
APuzzlePedestal::APuzzlePedestal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PedestalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PedestalMesh"));
	RootComponent = PedestalMesh;
	
	OverlapZone = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapZone"));
	OverlapZone->SetupAttachment(RootComponent);
	OverlapZone->SetCollisionProfileName(TEXT("Trigger"));

}

// Called when the game starts or when spawned
void APuzzlePedestal::BeginPlay()
{
	Super::BeginPlay();
	OverlapZone->OnComponentBeginOverlap.AddDynamic(this, &APuzzlePedestal::OnZoneBeginOverlap);
}

void APuzzlePedestal::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsActivated || !OtherActor || OtherActor == this) {return;}
	
	AABaseThowableItem* Item = Cast<AABaseThowableItem>(OtherActor);
	if (!Item) {return;}
	
	FGameplayTagContainer PassedTags = Item->DamageTags;
	
	for (int32 i = 0; i < RequiredSlots.Num(); ++i)
	{
		FPedestalSlot& Slot = RequiredSlots[i];
		
		if (Slot.bIsFilled || !PassedTags.HasTag(Slot.RequiredTag)) {continue;}
		
		ForceReleaseItem(OtherActor);
		DisableItemPhysics(OtherActor);
		AttachItemToSlot(OtherActor, Slot, i);
			
		if (ItemInsertedSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ItemInsertedSound, GetActorLocation());
		}
		break;
		
	}
}

bool APuzzlePedestal::CheckIfAllSlotsFilled() const
{
	for (const FPedestalSlot& Slot : RequiredSlots)
	{
		if (!Slot.bIsFilled) return false;
	}
	return true;
}

void APuzzlePedestal::ActivatePedestal()
{
	if (bIsActivated) {return;}
	if (PedestalActivatedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PedestalActivatedSound, GetActorLocation());
	}
	OnPedestalActivated.Broadcast(this);
}

void APuzzlePedestal::ForceReleaseItem(AActor* OtherActor)
{
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (UGrabberComponent* GrabberComp = PlayerPawn->FindComponentByClass<UGrabberComponent>())
		{
			if (GrabberComp && GrabberComp->GetHeldComponent() && GrabberComp->GetHeldComponent()->GetOwner() == OtherActor)
			{
				GrabberComp->ReleaseHeldObject();
			}
		}
	}
}

void APuzzlePedestal::DisableItemPhysics(AActor* OtherActor)
{
	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent()))
	{
		PrimComp->SetSimulatePhysics(false);
		PrimComp->SetCollisionProfileName(TEXT("NoCollision"));	
	}
	if (UProjectileMovementComponent* PMC = OtherActor->FindComponentByClass<UProjectileMovementComponent>())
	{
		PMC->StopMovementImmediately();
		PMC->Deactivate();
	}
	OtherActor->SetActorTickEnabled(false);
}

void APuzzlePedestal::AttachItemToSlot(AActor* OtherActor, FPedestalSlot& Slot, int32 i)
{
	OtherActor->AttachToComponent(PedestalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Slot.AttachSocketName);
	Slot.bIsFilled = true;
	Slot.AttachedItem = OtherActor;
	
	OnSlotFilled(i, OtherActor);
	if (CheckIfAllSlotsFilled())
	{
		ActivatePedestal();
		bIsActivated = true;
	}
}



