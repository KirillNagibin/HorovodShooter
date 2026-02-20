// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "SplineShapeSettings.generated.h"

UENUM(BlueprintType)
enum class EHorovodShape : uint8
{
	Circle UMETA(DisplayName = "Circle"),
	Square UMETA(DisplayName = "Square"),
	Star UMETA(DisplayName = "Star"),
	Custom UMETA(DisplayName = "Custom"),
	Spiral UMETA(DisplayName = "Spiral"),
	Eight UMETA(DisplayName = "Eight"),
	SineWave UMETA(DisplayName = "SineWave"),
};

USTRUCT(BlueprintType)
struct FSplineShapeSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	EHorovodShape ShapeType = EHorovodShape::Custom;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	TEnumAsByte<ESplinePointType::Type> PointType = ESplinePointType::Curve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	float Radius = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape", meta = (ClampMin = "3"))
	int32 PointsCount = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape", meta = (ClampMin = "0.1", ClampMax = "1", EditCondition = "ShapeType = EHorovodShape::Star"))
	float StarInnerRatio = 0.4f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape")
	float CycleWidth = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape", meta = (ClampMax = "360"))
	float PointsPerCycle = 5;
	
};
