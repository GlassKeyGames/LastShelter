// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HittingResources.generated.h"

class UStaticMeshComponent;
class AActor;

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	Wood,
	Stone
};

UCLASS()
class END2508_API AHittingResources : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHittingResources();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;

    // Set per BP (Tree = Wood, Rock = Stone)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    EResourceType ResourceType = EResourceType::Wood;

    // How much player gets per valid hit
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource", meta = (ClampMin = "1"))
    int32 AmountPerHit = 1;

    // Optional cooldown so one swing doesn't trigger multiple overlaps
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource", meta = (ClampMin = "0.0"))
    float HitCooldown = 0.15f;

    UFUNCTION(BlueprintCallable, Category = "Resource")
    void HandleResourceHit(AActor* PlayerActor);

    UFUNCTION(BlueprintCallable, Category = "Resource")
    EResourceType GetResourceType() const { return ResourceType; }

private:
    float LastHitTime = -1000.f;

};
