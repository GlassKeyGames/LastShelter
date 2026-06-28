// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USoundBase;
class UStaticMesh;

UCLASS()
class END2508_API ADoor : public AActor
{
	GENERATED_BODY()

public:
	ADoor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	USceneComponent* Hinge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	bool bIsOpen = false;

	FRotator ClosedRotation;
	FRotator OpenRotation;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FRotator ClosedRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FRotator OpenRotationOffset = FRotator(0.f, 90.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Offsets")
	FVector DoorBaseRelativeLocation = FVector(0.f, 55.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Offsets")
	FVector ClosedOnlyRelativeLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Offsets")
	FVector OpenDoorRelativeLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Sound")
	USoundBase* OpenDoorSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Sound")
	USoundBase* CloseDoorSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	UStaticMesh* DoorAsset = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Door")
	void ToggleDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	bool IsOpen() const { return bIsOpen; }

	UStaticMeshComponent* GetDoorMesh() const { return DoorMesh; }
};