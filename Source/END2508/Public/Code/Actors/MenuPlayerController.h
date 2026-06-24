// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class END2508_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Widget class set in Blueprint (WBP_MainMenu)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MenuClass;

private:
	UPROPERTY()
	UUserWidget* MenuInstance;
};
