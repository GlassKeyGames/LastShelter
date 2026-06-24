// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USoundBase;

UCLASS()
class END2508_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	USceneComponent* Hinge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UStaticMeshComponent* DoorMesh;

	FRotator ClosedRotation;
	FRotator OpenRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	bool bIsOpen = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenYawOffset = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FVector HingeOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FRotator ClosedRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FRotator OpenRotationOffset = FRotator(0.f, 90.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Sound")
	USoundBase* OpenDoorSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Sound")
	USoundBase* CloseDoorSound = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Door")
	void ToggleDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	bool IsOpen() const { return bIsOpen; }

	UStaticMeshComponent* GetDoorMesh() const { return DoorMesh; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	UStaticMesh* DoorAsset = nullptr;

};
 