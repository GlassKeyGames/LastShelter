// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/EnemyReload.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Code/Utility/CodeInterface.h" 

UEnemyReload::UEnemyReload()
{
    NodeName = "Enemy Reload";
    MessageName = FName("ActionFinishedMessage");
    bNotifyTick = false;
    bNotifyTaskFinished = false;
}

EBTNodeResult::Type UEnemyReload::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
        return EBTNodeResult::Failed;

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
        return EBTNodeResult::Failed;

    // Check if it implements the interface
    if (AIPawn->GetClass()->ImplementsInterface(UCodeInterface::StaticClass()))
    {
        // Cast to interface and call method
        if (ICodeInterface* Interface = Cast<ICodeInterface>(AIPawn))
        {
            Interface->EnemyReload();
        }
        else
        {
            return EBTNodeResult::Failed;
        }
    }
    else
    {
        return EBTNodeResult::Failed;
    }

    // Wait for message from Notify (or animation end)
    OwnerComp.RegisterMessageObserver(this, MessageName);
    return EBTNodeResult::InProgress;
}

void UEnemyReload::OnActionFinished(UBehaviorTreeComponent* OwnerComp)
{
    OwnerComp->UnregisterMessageObserversFrom(this); // Clean up
    FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded); // <-- Must pass by reference
}
