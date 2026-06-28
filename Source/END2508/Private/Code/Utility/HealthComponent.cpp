// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Utility/HealthComponent.h"
#include "Both/CharacterAnimation.h"
#include "Code/Actors/BaseCharacter.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    OwningCharacter = Cast<ABaseCharacter>(GetOwner());
}

void UHealthComponent::TakeDamage(float DamageAmount)
{
    if (DamageAmount <= 0.f)
    {
        return;
    }

    if (CurrentHealth <= 0.f)
    {
        return;
    }

    float NewHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
    CurrentHealth = NewHealth;

    float Ratio = (MaxHealth > 0.f) ? CurrentHealth / MaxHealth : 0.f;

    if (CurrentHealth <= 0.f)
    {
        // Dead
        UE_LOG(LogTemp, Warning, TEXT("died!"), *GetOwner()->GetName());

        if (OwningCharacter)
        {
            OwningCharacter->UpdateHealthHUD(CurrentHealth, MaxHealth);
            OwningCharacter->OnPlayerDeath(); 
        }
    }
    else
    {
        // Alive and hurt
        OnHurt.Broadcast(Ratio);  

        UE_LOG(LogTemp, Warning, TEXT("Took damage, ratio = %.2f"), Ratio);

        if (OwningCharacter)
        {
            OwningCharacter->UpdateHealthHUD(CurrentHealth, MaxHealth);

            if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(OwningCharacter->GetMesh()->GetAnimInstance()))
            {
                Anim->HitAnimation();
            }
        }
    }
}

void UHealthComponent::Heal(float HealAmount)
{
    if (HealAmount <= 0.f || CurrentHealth >= MaxHealth)
    {
        return;
    }

    CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, MaxHealth);
    float Ratio = (MaxHealth > 0.f) ? CurrentHealth / MaxHealth : 0.f;

    OnHeal.Broadcast(Ratio);  

    if (OwningCharacter)
    {
        OwningCharacter->UpdateHealthHUD(CurrentHealth, MaxHealth);
    }
}

void UHealthComponent::HandleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    TakeDamage(Damage);
}

void UHealthComponent::ResetHealth()
{
    CurrentHealth = MaxHealth;
}