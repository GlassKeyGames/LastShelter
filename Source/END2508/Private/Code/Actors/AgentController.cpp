// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/AgentController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AIPerceptionTypes.h"
#include "../END2508.h"
#include "Code/Actors/BaseCharacter.h"
#include "Code/Utility/HealthComponent.h"
#include "Perception/AISense_Sight.h"
#include "Code/Utility/CodeInterface.h"

AAgentController::AAgentController()
{
	// Create Blackboard
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	// Setup sight config
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// Setup perception component
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*PerceptionComponent);
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAgentController::OnPerceptionUpdated);
}

void AAgentController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BBAsset, BlackboardComp))
	{
		RunBehaviorTree(BTAsset);
	}
}

void AAgentController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!BlackboardComp || !Actor) return;

	if (!Actor->ActorHasTag(FName("Player")))
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		BlackboardComp->SetValueAsObject(PlayerKeyName, Actor);
	}

	UObject* CheckValue = BlackboardComp->GetValueAsObject(PlayerKeyName);
	FString CheckName = CheckValue ? CheckValue->GetName() : TEXT("None");

}