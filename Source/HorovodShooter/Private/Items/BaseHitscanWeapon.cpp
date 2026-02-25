// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BaseHitscanWeapon.h"

#include "Interfaces/DamagableInterface.h"
#include "HorovodShooter/HorovodShooter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ABaseHitscanWeapon::ABaseHitscanWeapon()
{
	bUsePhysicsHold = false;
	bCanBeThrownManually = false;
	
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	
	if (GetRootComponent())
	{
		MuzzleLocation->SetupAttachment(GetRootComponent()); 
	}
}

void ABaseHitscanWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = MaxAmmo;
	UE_LOG(LogTemp, Warning, TEXT("[WEAPON DEBUG] BeginPlay: %s. MaxAmmo: %d, CurrentAmmo Set To: %d"), *GetName(), MaxAmmo, CurrentAmmo);

}

void ABaseHitscanWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bUsePhysicsHold && GetRootComponent() && GetRootComponent()->GetAttachParent())
	{
		ProcessWallAvoidance(DeltaTime);
	}
}

bool ABaseHitscanWeapon::OnUsePressed_Implementation()
{
UE_LOG(LogHorovod, Warning, TEXT("[WEAPON DEBUG] OnUsePressed Called. CurrentAmmo: %d"), CurrentAmmo);
	if (CurrentAmmo <= 0)
	{
		return false;
	}
	PerformFire();
	CurrentAmmo--;
	UE_LOG(LogHorovod, Warning, TEXT("Ammo left = %d"), CurrentAmmo);
	
	if (CurrentAmmo <= 0)
	{
		UE_LOG(LogHorovod, Warning, TEXT("No ammo left, throwing weapon"));
		bIsArmedToExplode = true;
	}
	return true;
}

void ABaseHitscanWeapon::HandleImpact_Implementation(const FHitResult& Hit)
{
	if (bIsArmedToExplode)
	{
		Explode();
		Destroy();
	}
	else
	{
		Super::HandleImpact_Implementation(Hit);
	}
	
}

void ABaseHitscanWeapon::Explode()
{
	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation(),
		ExplosionRadius,
		ObjectTypes,
		AActor::StaticClass(),
		{this, GetOwner()},
		OverlappedActors
		);
	
	FCollisionQueryParams ExplosionParams;
	ExplosionParams.AddIgnoredActor(this);
	ExplosionParams.AddIgnoredActors(OverlappedActors);
	
	for (AActor* Victim : OverlappedActors)
	{
		if (!Victim) {continue;}
		FHitResult Hit;
		
		bool bBlocked = GetWorld()->LineTraceSingleByChannel(
			Hit,
			GetActorLocation(),
			Victim->GetActorLocation(),
			ECC_Visibility,
			ExplosionParams 
			);
		if (!bBlocked || Hit.GetActor() == Victim)
		{
			if (Victim->Implements<UDamagableInterface>())
			{
				IDamagableInterface::Execute_TakeDamage(Victim, this->DamageTags);
			}
		}
	}
OnExplode();
}

void ABaseHitscanWeapon::ProcessWallAvoidance(float DeltaTime)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) {return;}
	
	//Ведем луч из глаз игрока
	FVector ViewLocation;
	FRotator ViewRotation;
	
	PC->GetPlayerViewPoint(ViewLocation, ViewRotation);
	
	FVector Start = ViewLocation + (ViewRotation.Vector() * 10.0f);
	FVector End = Start + (ViewRotation.Vector() * WeaponLength);
	
	//Настраиваем коллизию луча
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	FHitResult Hit;
	float AvoidanceRadius = 40.0;
	bool bHit = GetWorld()->SweepSingleByChannel(
			Hit,
			Start,
			End,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility,
			FCollisionShape::MakeSphere(AvoidanceRadius),
			Params
			);
	
	float TargetOffset = 0.0f;
	
	if (bHit)
	{
		float DistTowall = (Hit.Location - Start).Size();
		TargetOffset = FMath::Clamp(WeaponLength - DistTowall, 0.0f, WeaponLength);
	}
	CurrentRetractOffset = FMath::FInterpTo(CurrentRetractOffset, TargetOffset, DeltaTime, RetractionSpeed);
	
	SetActorRelativeLocation(FVector(-CurrentRetractOffset, 0.0f, 0.0f));
}

void ABaseHitscanWeapon::PerformFire()
{
	//Здесь получили контроллер игрока
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) {return;}
	
	//Ведем луч из глаз игрока
	FVector ViewLocation;
	FRotator ViewRotation;
	
	PC->GetPlayerViewPoint(ViewLocation, ViewRotation);
	
	FVector Start = ViewLocation;
	FVector End = Start + (ViewRotation.Vector() * ShotRange);
	
	//Настраиваем коллизию луча
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	
	FHitResult Hit;
	//DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 2.0f, 0, 2);
	
	int32 PenetrationCount = 0;
	FVector BeamEndLocation = End;
	
	while (PenetrationCount <= PiercingCount)
	{
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			Hit,
			Start,
			End,
			ECC_Visibility,
			Params
			);
		if (bHit)
		{
			AActor* HitActor = Hit.GetActor();
			BeamEndLocation = Hit.Location;
			if (HitActor)
			{
				if (HitActor->Implements<UDamagableInterface>())
				{
					IDamagableInterface::Execute_TakeDamage(HitActor, this->DamageTags);
					Params.AddIgnoredActor(HitActor);
					PenetrationCount++;
					continue;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	
	bool bIsLastShot = (CurrentAmmo == 1);
	OnFire(MuzzleLocation->GetComponentLocation(), End, bIsLastShot);
}




