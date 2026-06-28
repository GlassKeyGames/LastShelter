// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Utility/RegenHealthComponent.h"
#include "../END2508.h"

URegenHealthComponent::URegenHealthComponent()
{
	MaxHealth = 11;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	OnDeath.AddDynamic(this, &URegenHealthComponent::HandleDeath);
}

void URegenHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryComponentTick.SetTickFunctionEnable(true);

}

void URegenHealthComponent::HandleDeath(float Ratio)
{

	TurnTickOff();  
}

void URegenHealthComponent::TurnTickOff()
{
	PrimaryComponentTick.SetTickFunctionEnable(false);
	GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);
}

void URegenHealthComponent::TurnTickOn()
{
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void URegenHealthComponent::HandleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Super::HandleDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser); // ?? no error now

	if (Damage > 0 && IsAlive())
	{
		GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &URegenHealthComponent::TurnTickOff, 2.f);
	}

	TurnTickOn();
}

void URegenHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Use TakeDamage with a negative value to heal
	Heal(1.f * DeltaTime);

	if(IsFullHealth())
	{
		TurnTickOff();
	}
}
