// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class ABaseCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthEvent, float, Ratio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathEvent, float, Ratio);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class END2508_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UHealthComponent();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float CurrentHealth = 0.f;

    UPROPERTY()
    ABaseCharacter* OwningCharacter;

    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthEvent OnHurt;

    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthEvent OnHeal;

    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDeathEvent OnDeath;

    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual void TakeDamage(float Damage);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ResetHealth();

    float GetMax() const { return MaxHealth; }
    float GetCurrent() const { return CurrentHealth; }

	FORCEINLINE bool IsFullHealth() const { return CurrentHealth / MaxHealth == 1.f; }
	FORCEINLINE bool IsAlive() const { return CurrentHealth > 0.f; }

    virtual void HandleDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
