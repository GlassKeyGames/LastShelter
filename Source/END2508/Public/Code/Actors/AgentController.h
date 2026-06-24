// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AgentController.generated.h"

class UAIPerceptionComponent;
class UBehaviorTree;
class UBlackboardComponent;
class UAISenseConfig_Sight;

UCLASS()
class END2508_API AAgentController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAgentController();
protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BTAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBlackboardData* BBAsset;

	// Blackboard keys 
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName PlayerKeyName = TEXT("Player");
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName LocationKeyName = TEXT("Location");
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName HealthKeyName = TEXT("Health Ratio");   

protected:
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComp;
private:
	bool bCanSeePlayer = false;
	FTimerHandle SightLossDelayHandle;
};
