// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class END2508_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void LoadFirstLevel();

	UFUNCTION(BlueprintCallable)
	void QuitTheGame();

	UFUNCTION(BlueprintCallable)
	void LoadCurrentLevel();

	//UFUNCTION(BlueprintCallable)
	//oid LoadLevelSafe(int32 LevelIndex);

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	// List of level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	TArray<FName> GameLevels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	int32 FirstLevelIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Levels")
	int32 CurrentLevelIndex = 0;
};
