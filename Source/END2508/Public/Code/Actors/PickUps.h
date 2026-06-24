// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Code/EquipedState.h"
#include "Sound/SoundBase.h"
#include "PickUps.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UTexture2D;
class ABasePlayer;

UCLASS()
class END2508_API APickUps : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUps();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:	
	/** Called by player when E is pressed while targeting this pickup */
	bool TryPickup(ABasePlayer* Player);

	/** Collision trigger */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	USphereComponent* PickupSphere;

	/** Visible mesh in world */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup|Visual")
	USkeletalMeshComponent* PickupSkeletalMesh;

	/** What equip state this grants */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	EEquipState PickupType = EEquipState::Unarmed;

	/** Icon shown in hotbar */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	UTexture2D* PickupIcon = nullptr;

	/** Class stored in slot metadata */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TSubclassOf<AActor> PickupItemClass = nullptr;

	/** UI prompt text */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	FText PickupText = FText::FromString(TEXT("Pick Up"));

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	AActor* WorldWeaponActor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* PickupSound = nullptr;

};
