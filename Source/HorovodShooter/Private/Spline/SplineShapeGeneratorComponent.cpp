// Fill out your copyright notice in the Description page of Project Settings.


#include "Spline/SplineShapeGeneratorComponent.h"
#include "Components/SplineComponent.h"

// Sets default values for this component's properties
USplineShapeGeneratorComponent::USplineShapeGeneratorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USplineShapeGeneratorComponent::GenerateSpline(USplineComponent* TargetSpline)
{
	if (!TargetSpline) {return;}
	
	if (SplineShapeSettings.ShapeType == EHorovodShape::Custom) {return;}
	
	TargetSpline->ClearSplinePoints(true);
	
	switch (SplineShapeSettings.ShapeType)
	{
	case EHorovodShape::Circle:
		BuildCircle(TargetSpline);
		break;
	case EHorovodShape::Square:
		BuildSquare(TargetSpline);
		break;
	case EHorovodShape::Star:
		BuildStar(TargetSpline);
		break;
	case EHorovodShape::Spiral:
		BuildSpiral(TargetSpline);
		break;
	case EHorovodShape::Eight:
		BuildEight(TargetSpline);
		break;
	case EHorovodShape::SineWave:
		BuildSineWave(TargetSpline);
		break;
	default:
		break;
	}
}

void USplineShapeGeneratorComponent::GenerateSplineForOwner()
{
	if (AActor* Owner = GetOwner())
	{
		if (USplineComponent* Spline = Owner->FindComponentByClass<USplineComponent>())
		{
			GenerateSpline(Spline);
		}
	}
}

void USplineShapeGeneratorComponent::BuildCircle(USplineComponent* Spline)
{
	Spline->ClearSplinePoints();
	float AngleStep = UE_TWO_PI / static_cast<float>(FMath::Max(3, SplineShapeSettings.PointsCount));
	
	for (int32 i = 0; i < SplineShapeSettings.PointsCount; i++)
	{
		float CurrentAngle = i * AngleStep;
		float X = FMath::Cos(CurrentAngle) * SplineShapeSettings.Radius;
		float Y = FMath::Sin(CurrentAngle) * SplineShapeSettings.Radius;
		FVector PointLocation(X, Y,0);
		
		
		Spline->AddSplinePoint(PointLocation, ESplineCoordinateSpace::Local, false);
		Spline->SetSplinePointType(i, SplineShapeSettings.PointType, false);
	}
	Spline->SetClosedLoop(true);
}

void USplineShapeGeneratorComponent::BuildSquare(USplineComponent* Spline)
{
	Spline->ClearSplinePoints();
	FVector Points[] = 
	{
		FVector(SplineShapeSettings.Radius, SplineShapeSettings.Radius, 0.0),
		FVector(SplineShapeSettings.Radius, -SplineShapeSettings.Radius, 0.0),
		FVector(-SplineShapeSettings.Radius, -SplineShapeSettings.Radius, 0.0),
		FVector(-SplineShapeSettings.Radius, SplineShapeSettings.Radius, 0.0),
	};
	for (int i = 0; i < 4; i++)
	{
	
		Spline->AddSplinePoint(Points[i], ESplineCoordinateSpace::Local, false);
		Spline->SetSplinePointType(i, SplineShapeSettings.PointType, false);
	}
	Spline->SetClosedLoop(true);
}

void USplineShapeGeneratorComponent::BuildStar(USplineComponent* Spline)

{
	int32 Rays = FMath::Max(3, SplineShapeSettings.PointsCount);
	int32 TotalPoints = Rays * 2;
	
	float AngleStep = UE_TWO_PI / (float) TotalPoints;
	
	for (int32 i = 0; i < TotalPoints; i++)
	{
		float Angle = i * AngleStep;
		
		float CurrentRadius =  (i % 2 == 0) ? SplineShapeSettings.Radius : (SplineShapeSettings.Radius * SplineShapeSettings.StarInnerRatio);
		
		FVector Point 
			(
				FMath::Cos(Angle) * CurrentRadius,
				FMath::Sin(Angle) * CurrentRadius,
				0.0f
			);
		
		Spline->AddSplinePoint(Point, ESplineCoordinateSpace::Local, false);
		Spline->SetSplinePointType(i, SplineShapeSettings.PointType, false);
	}
	Spline->SetClosedLoop(true);
}

void USplineShapeGeneratorComponent::BuildSpiral(USplineComponent* Spline)
{
	Spline->ClearSplinePoints(false);
	
	float AngleStep = UE_TWO_PI / SplineShapeSettings.PointsPerCycle;
	
	for (int32 i = 0; i < SplineShapeSettings.PointsCount; i++)
	{
		float CurrentAngle = i * AngleStep;
		float X = FMath::Cos(CurrentAngle) * SplineShapeSettings.Radius;
		float Y = FMath::Sin(CurrentAngle) * SplineShapeSettings.Radius;
		float Z = SplineShapeSettings.CycleWidth * i;
		FVector PointLocation(X, Y, Z);
		
		
		Spline->AddSplinePoint(PointLocation, ESplineCoordinateSpace::Local, false);
		Spline->SetSplinePointType(i, SplineShapeSettings.PointType, false);
	}
	Spline->SetClosedLoop(false, true);
	UE_LOG(LogTemp,Warning, TEXT("SpiralSuccess"));
}

void USplineShapeGeneratorComponent::BuildEight(USplineComponent* Spline)
{
	Spline->ClearSplinePoints();
	int32 Points = FMath::Max(16, SplineShapeSettings.PointsCount);
	float AngleStep = UE_TWO_PI /static_cast<float>(Points);
	
	for (int32 i = 0; i <= Points; i++)
	{
		float CurrentAngle = i * AngleStep;
		FVector Point 
		(
			FMath::Cos(CurrentAngle) * SplineShapeSettings.Radius,
			(FMath::Sin(2.f * CurrentAngle) * SplineShapeSettings.Radius) * 0.5f,
			0.0f
			);
		
		Spline->AddSplinePoint(Point, ESplineCoordinateSpace::Local, false);
		Spline->SetSplinePointType(i, SplineShapeSettings.PointType, false);
	}
	Spline->SetClosedLoop(true, true);
}

void USplineShapeGeneratorComponent::BuildSineWave(USplineComponent* Spline)
{
	Spline->ClearSplinePoints();
	int32 Points = FMath::Max(4, SplineShapeSettings.PointsCount);
	float XStep = SplineShapeSettings.CycleWidth / Points;
	float Frequency =  UE_TWO_PI/SplineShapeSettings.PointsPerCycle;
	
	for (int32 i = 0; i < Points; i++)
	{
		
		float X = i * XStep;
		float Y = FMath::Sin(Frequency * i) * SplineShapeSettings.Radius;
		float Z = 0.0f;
		
		FVector Point(X, Y, Z);
		Spline->AddSplinePoint(Point, ESplineCoordinateSpace::Local, false);
		Spline->SetSplinePointType(i, SplineShapeSettings.PointType, false);
	}
	Spline->SetClosedLoop(false, true);
}


// Called when the game starts


