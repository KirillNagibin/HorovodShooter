// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		
		if (AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), PlayerActor);
		}
	}
}
