// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BTService_DistanceCheck.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"


UBTService_DistanceCheck::UBTService_DistanceCheck()
{
	NodeName = TEXT("CheckTargetDistance");
	
	bNotifyTick = true;
	
	bCallTickOnSearchStart = true;
	
	Interval = 0.5f;
	RandomDeviation = 0.1f; 
}

//Every frame enemy checks distance to the target to evaluate his next move
void UBTService_DistanceCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if (!IsValid(AIController) || !IsValid(BlackboardComp)) return;
	
	APawn* AIPawn = AIController->GetPawn();
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
	
	if (!IsValid(TargetActor))
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
		if (IsValid(TargetActor))
		{
			BlackboardComp->SetValueAsObject(TargetKey.SelectedKeyName, TargetActor);
		}
	}
	
	if (!IsValid(AIPawn) || !IsValid(TargetActor)) return;
	
	float Distance = FVector::DistXY(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());
	bool bNeedToChase = (Distance >= MaxDistance);
	bool bNeedToDash = (Distance <= MinDistance);
	BlackboardComp->SetValueAsBool(OutOfRangeKey.SelectedKeyName, bNeedToChase);
	BlackboardComp->SetValueAsBool(TooCloseKey.SelectedKeyName, bNeedToDash);
	
	if (bNeedToChase)
	{
		FVector CurrentMoveLocation = BlackboardComp->GetValueAsVector(MoveLocationKey.SelectedKeyName);
		if (!CurrentMoveLocation.IsNearlyZero())
		{
			float DistToMoveLocation = FVector::Distance(TargetActor->GetActorLocation(), CurrentMoveLocation);
			if (DistToMoveLocation < (MinDistance - Tolerance) || DistToMoveLocation > (MaxDistance + Tolerance))
			{
				BlackboardComp->ClearValue(MoveLocationKey.SelectedKeyName);
			}
		}
	}
}
