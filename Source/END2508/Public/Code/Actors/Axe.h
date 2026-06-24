// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Axe.generated.h"

UCLASS()
class END2508_API AAxe : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAxe();

	// Call this after spawning/attaching so the axe knows who owns it
	void InitializeAxe(AActor* InOwner);

	// Called when player presses attack
	UFUNCTION(BlueprintCallable)
	virtual bool Attack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	// Use ONE of these (pick whichever your axe asset uses)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float Range = 180.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float Radius = 35.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float Damage = 25.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

private:
	UPROPERTY()
	AActor* OwnerActor = nullptr;

};
