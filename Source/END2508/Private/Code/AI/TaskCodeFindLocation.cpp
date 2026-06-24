// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/AI/TaskCodeFindLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Math/UnrealMathUtility.h"  // for FMath
#include "Kismet/KismetMathLibrary.h"

UTaskCodeFindLocation::UTaskCodeFindLocation()
{
    NodeName = "Find Random Location (No NavMesh)";
    Location.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UTaskCodeFindLocation, Location));
}

EBTNodeResult::Type UTaskCodeFindLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!ControlledPawn) return EBTNodeResult::Failed;

    const FVector Origin = ControlledPawn->GetActorLocation();

    // Generate random point within a radius (X/Y only, keep same Z height)
    FVector2D Random2D = FMath::RandPointInCircle(Radius);
    FVector RandomLocation = Origin + FVector(Random2D.X, Random2D.Y, 0.f); // keep Z constant

    OwnerComp.GetBlackboardComponent()->SetValueAsVector(Location.SelectedKeyName, RandomLocation);

    return EBTNodeResult::Succeeded;
}