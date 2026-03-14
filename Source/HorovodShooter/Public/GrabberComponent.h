// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Interfaces/UsableInterface.h"
#include "GrabberComponent.generated.h"

//Заранее объявляем класс, чтобы не включать всю библиотеку. Это было бы слишком тяжеловесно
class UPhysicsHandleComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOROVODSHOOTER_API UGrabberComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabberComponent();

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> HeldComponent = nullptr;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Прописываем все функции, которые будем вызывать в c++ файле
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void AttemptGrab();
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ThrowHeldObject();
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ReleaseHeldObject();
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PrimaryAction();
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UPrimitiveComponent* GetHeldComponent() const {return HeldComponent;}
	
protected:
	//Компоненты и переменные
	UPROPERTY()
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandle;
	

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MaxGrabDistance = 400.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ThrowForce = 150000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float GrabRadius = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float HoldDistance = 150.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bDrawDebugLines = true;
	
private:
	void SetupPhysicsHandle();
	FHitResult GetPhysicsBodyInReach() const;
	
};
