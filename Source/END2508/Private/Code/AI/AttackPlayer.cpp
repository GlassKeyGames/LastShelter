// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/AI/AttackPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Code/Actors/Agent.h"
#include "Code/Utility/CodeInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UAttackPlayer::UAttackPlayer()
{
	NodeName = TEXT("Attack Player");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UAttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	AAgent* Agent = Cast<AAgent>(Pawn);
	if (!Agent) return EBTNodeResult::Failed;

	ControlledPawn = Pawn;
	CachedOwnerComp = &OwnerComp;

	Agent->OnActionStopped.RemoveAll(this);
	Agent->OnActionStopped.AddUObject(this, &UAttackPlayer::OnActionFinished);

	return EBTNodeResult::InProgress;
}

void UAttackPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	AAIController* AICon = OwnerComp.GetAIOwner();
	AAgent* Agent = AICon ? Cast<AAgent>(AICon->GetPawn()) : nullptr;
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Agent, 0);


	if (!AICon || !Agent || !Player)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float Dist2D = FVector::Dist2D(Agent->GetActorLocation(), Player->GetActorLocation());

	// If not close enough, keep moving in
	if (Dist2D > Agent->MeleeRange)
	{
		AICon->MoveToActor(Player, 5.f);
		return;
	}

	// Close enough: stop and attack
	AICon->StopMovement();

	if (Agent->bIsAttacking)
	{
		return;
	}

	if (ICodeInterface* Interface = Cast<ICodeInterface>(Agent))
	{
		Interface->EnemyAttack();
	}
}

void UAttackPlayer::OnActionFinished()
{
	if (ControlledPawn.IsValid() && CachedOwnerComp)
	{
		if (AAgent* Agent = Cast<AAgent>(ControlledPawn.Get()))
		{
			Agent->OnActionStopped.RemoveAll(this);
		}

		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}