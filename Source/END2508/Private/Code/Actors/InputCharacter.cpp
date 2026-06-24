// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/InputCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AInputCharacter::Look(const FInputActionValue& Value)
{
	if (EInputActionValueType::Axis2D == Value.GetValueType())
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AInputCharacter::Move(const FInputActionValue& Value)
{
	if (EInputActionValueType::Axis2D == Value.GetValueType())
	{
		FVector2D MovementVector = Value.Get<FVector2D>();
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AInputCharacter::Shoot()
{
	OnShoot();
}

// Sets default values
AInputCharacter::AInputCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInputCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
}

// Called every frame
void AInputCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AInputCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInputCharacter::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInputCharacter::Move);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AInputCharacter::Shoot);
		//EnhancedInputComponent->BindAction(BuildModeAction, ETriggerEvent::Started, this, &AInputCharacter::BuildMode);
	}

}

