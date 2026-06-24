// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BarrelSpawner.generated.h"

UCLASS()
class END2508_API ABarrelSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABarrelSpawner();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BarrelMesh;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    TSubclassOf<APawn> AgentToSpawn;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    float SpawnDelay = 20.f;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    float MaxHealth = 100.f;

    float CurrentHealth = 100.f;
    bool bIsDestroyed = false;

    FTimerHandle SpawnTimer;

    void SpawnAgent();
    void HandleDestroyed();

public:	
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

};
