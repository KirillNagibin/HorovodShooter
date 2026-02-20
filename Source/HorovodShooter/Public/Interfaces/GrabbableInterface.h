// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GrabbableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGrabbableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HOROVODSHOOTER_API IGrabbableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Функции, определяющте, что происходит с предметом, если мы его хватаем, отпускаем, бросаем
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void OnGrabbed(USceneComponent* GrabberComponent);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void OnReleased(AActor* Releaser);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void OnThrown(FVector Direction, float Magnitude);
};
