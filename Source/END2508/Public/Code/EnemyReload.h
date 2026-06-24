// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyReload.generated.h"

/**
 * 
 */
UCLASS()
class END2508_API UEnemyReload : public UBTTaskNode
{
	GENERATED_BODY()
public:

    UEnemyReload();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    FName MessageName;

    UFUNCTION()
    void OnActionFinished(UBehaviorTreeComponent* OwnerComp);
};
