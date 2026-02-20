// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Horovod/HorovodManager.h"
#include "HorovodDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class HOROVODSHOOTER_API UHorovodDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FHorovodWagonData WagonData;
};
