// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DashToPoint.generated.h"

/**
 * 
 */
UCLASS()
class HOROVODSHOOTER_API UBTT_DashToPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_DashToPoint();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetLocationKey;
};
