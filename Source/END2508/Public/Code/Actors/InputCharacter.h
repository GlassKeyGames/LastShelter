// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputCharacter.generated.h"

UCLASS()
class END2508_API AInputCharacter : public ACharacter
{
	GENERATED_BODY()

	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void Shoot();
public:
	// Sets default values for this character's properties
	AInputCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* ShootAction;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	//class UInputAction* BuildModeAction;

	UFUNCTION(BlueprintImplementableEvent)
	void OnShoot();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
