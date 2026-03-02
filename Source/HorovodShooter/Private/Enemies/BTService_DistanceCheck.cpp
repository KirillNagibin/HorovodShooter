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
	
	float Distance = FMath::Abs(FVector::Distance(AIPawn->GetActorLocation(), TargetActor->GetActorLocation()));
	
	bool bNeedToMove = (Distance <= MinDistance || Distance >= MaxDistance);
	BlackboardComp->SetValueAsBool(OutOfRangeKey.SelectedKeyName, bNeedToMove);
}
