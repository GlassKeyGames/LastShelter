// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/MyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Code/Widgets/Results.h"
#include "Code/Actors/BarrelSpawner.h"
#include "Code/Actors/BasePlayer.h" 
#include "Code/Actors/BaseCharacter.h" 

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    NumberOfEnemies = 0;
    NumberOfSpawners = 0;

    TArray<AActor*> FoundCharacters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), FoundCharacters);

    for (AActor* Actor : FoundCharacters)
    {
        ABaseCharacter* BaseChar = Cast<ABaseCharacter>(Actor);

        if (BaseChar && !BaseChar->IsPlayerControlled())
        {
            NumberOfEnemies++;
            Actor->OnDestroyed.AddDynamic(this, &AMyGameMode::HandleEnemyDestroyed);
        }
    }

    TArray<AActor*> Spawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABarrelSpawner::StaticClass(), Spawners);

    for (AActor* Spawner : Spawners)
    {
        NumberOfSpawners++;
        Spawner->OnDestroyed.AddDynamic(this, &AMyGameMode::HandleSpawnerDestroyed);
    }

    UE_LOG(LogTemp, Warning, TEXT("Enemies tracked: %d | Spawners tracked: %d"),
        NumberOfEnemies, NumberOfSpawners);

    if (ResultsWidgetClass)
    {
        ResultsWidgetObject = CreateWidget<UResults>(GetWorld(), ResultsWidgetClass);
        if (!ResultsWidgetObject)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create ResultsWidgetObject"));
        }
    }

    ABasePlayer* Player = Cast<ABasePlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (Player)
    {
        Player->OnPlayerLost.AddDynamic(this, &AMyGameMode::RemovePlayer);
        UE_LOG(LogTemp, Warning, TEXT("Bound GameMode::RemovePlayer to OnPlayerLost delegate"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to bind: could not get player pawn"));
    }
}

void AMyGameMode::HandleEnemyDestroyed(AActor* DestroyedActor)
{
    NumberOfEnemies = FMath::Max(0, NumberOfEnemies - 1);

    UE_LOG(LogTemp, Warning, TEXT("Enemy destroyed. Remaining enemies: %d | Remaining spawners: %d"),
        NumberOfEnemies, NumberOfSpawners);

    CheckWinCondition();
}

void AMyGameMode::RemovePlayer()
{
    UE_LOG(LogTemp, Warning, TEXT("You Lose!"));

    if (!ResultsWidgetObject)
    {
        UE_LOG(LogTemp, Error, TEXT("ResultsWidgetObject is null!"));
        return;
    }

    // Add the widget to the screen
    ResultsWidgetObject->AddToViewport();

    // Set input mode UI only
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }

    ResultsWidgetObject->SetLose();
}

void AMyGameMode::HandleSpawnerDestroyed(AActor* DestroyedSpawner)
{
    NumberOfSpawners = FMath::Max(0, NumberOfSpawners - 1);

    UE_LOG(LogTemp, Warning, TEXT("Spawner destroyed. Remaining enemies: %d | Remaining spawners: %d"),
        NumberOfEnemies, NumberOfSpawners);

    CheckWinCondition();
}

void AMyGameMode::CheckWinCondition()
{
    if (NumberOfEnemies <= 0 && NumberOfSpawners <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("YOU WIN!"));

        if (!ResultsWidgetObject)
        {
            UE_LOG(LogTemp, Error, TEXT("ResultsWidgetObject is null in CheckWinCondition"));
            return;
        }

        ResultsWidgetObject->AddToViewport();
        ResultsWidgetObject->SetWin();

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            PC->SetInputMode(FInputModeUIOnly());
            PC->bShowMouseCursor = true;
        }
    }
}
