// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/BasePickUp.h"
#include "Components/BoxComponent.h"


ABasePickUp::ABasePickUp()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);

	BoxCollision->SetGenerateOverlapEvents(true);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionObjectType(ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABasePickUp::BoundOverlap);
}

void ABasePickUp::BoundOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (CanPickup())
	{
		HandlePickUp(OtherActor, SweepResult); 
		PostPickUp();                          
	}
}

void ABasePickUp::HandlePickUp_Implementation(AActor* OtherActor, const FHitResult& SweepResult)
{
	
}

void ABasePickUp::PostPickUp_Implementation()
{
	Destroy(); 
}

bool ABasePickUp::CanPickup_Implementation() const
{
	return true; 
}
