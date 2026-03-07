// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BTT_DashToPoint.h"
#include "Enemies/StalkerEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/DashComponent.h"

UBTT_DashToPoint::UBTT_DashToPoint()
{
	NodeName = TEXT("DashToPoint");
}

EBTNodeResult::Type UBTT_DashToPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if (!BlackBoardComp || !AIController) return EBTNodeResult::Failed;
	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;
	
	FVector TargetLocation = BlackBoardComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);
	FVector DashDirection = (TargetLocation - AIPawn->GetActorLocation().GetSafeNormal());
	DashDirection.Z = 0.0f;
	
	AStalkerEnemy* Stalker = Cast<AStalkerEnemy>(AIPawn);
	if (Stalker)
	{
		UDashComponent* DashComp = Stalker->FindComponentByClass<UDashComponent>();
		if (DashComp)
		{
			DashComp->PerformDash(DashDirection);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}

