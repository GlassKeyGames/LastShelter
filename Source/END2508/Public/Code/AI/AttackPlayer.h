// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AttackPlayer.generated.h"

/**
 * 
 */
UCLASS()
class END2508_API UAttackPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UAttackPlayer();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	void OnActionFinished();
protected:

	TWeakObjectPtr<APawn> ControlledPawn;
	UBehaviorTreeComponent* CachedOwnerComp = nullptr;

};
