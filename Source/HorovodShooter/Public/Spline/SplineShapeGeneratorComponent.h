// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Spline/SplineShapeSettings.h"
#include "SplineShapeGeneratorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOROVODSHOOTER_API USplineShapeGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USplineShapeGeneratorComponent();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineGenerator")
	FSplineShapeSettings SplineShapeSettings;
	
	UFUNCTION( BlueprintCallable, Category = "SplineGenerator")
	void GenerateSpline(USplineComponent* TargetSpline);
	
	UFUNCTION(CallInEditor, Category = "SplineGenerator")
	void GenerateSplineForOwner();
	
	
	private:
		void BuildCircle(USplineComponent* Spline);
    	void BuildSquare(USplineComponent* Spline);
    	void BuildStar(USplineComponent* Spline);
    	void BuildSpiral(USplineComponent* Spline);
    	void BuildEight(USplineComponent* Spline);
		void BuildSineWave(USplineComponent* Spline);
	
		
};
