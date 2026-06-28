// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Code/Widgets/Results.h" 
#include "Sound/SoundBase.h"
#include "MyGameMode.generated.h"

class UUserWidget;
class UResults;

UCLASS()
class END2508_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleEnemyDestroyed(AActor* DestroyedActor);

    UFUNCTION()
    void RegisterSpawnedEnemy(AActor* SpawnedEnemy);

    UFUNCTION()
    void RemovePlayer();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> ResultsWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    USoundBase* WinSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    USoundBase* LoseSound;

    UPROPERTY()
    UResults* ResultsWidgetObject;  

    // Number of enemies currently alive
    UPROPERTY(BlueprintReadOnly, Category = "Game")
    int32 NumberOfEnemies = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Game")
    int32 NumberOfSpawners = 0;

    UFUNCTION()
    void HandleSpawnerDestroyed(AActor* DestroyedSpawner);

    void CheckWinCondition();
};
