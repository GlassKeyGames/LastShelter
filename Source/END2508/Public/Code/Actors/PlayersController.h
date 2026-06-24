// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PlayersController.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class END2508_API APlayersController : public APlayerController
{
	GENERATED_BODY()
	
public:
    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_Pause;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* GameplayMappingContext;

    UFUNCTION()
    void OnPauseTriggered(const FInputActionValue& Value);

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> PauseMenuClass;

    UPROPERTY()
    UUserWidget* PauseMenuInstance = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    bool bIsPaused = false;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowPauseMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HidePauseMenu();
};
