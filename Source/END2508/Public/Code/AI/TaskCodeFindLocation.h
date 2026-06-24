// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "TaskCodeFindLocation.generated.h"

/**
 * 
 */
UCLASS()
class END2508_API UTaskCodeFindLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
    UTaskCodeFindLocation();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    //Blackboard key to write the location to 
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    struct FBlackboardKeySelector Location;

    UPROPERTY(EditAnywhere, Category = "FindLocation", meta = (ClampMin = "0"))
    float Radius = 1000.f;

};
