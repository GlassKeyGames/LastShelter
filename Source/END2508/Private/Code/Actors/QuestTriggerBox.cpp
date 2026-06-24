// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/QuestTriggerBox.h"
#include "Components/BoxComponent.h"
#include "Code/Actors/BasePlayer.h"

// Sets default values
AQuestTriggerBox::AQuestTriggerBox()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);

	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionObjectType(ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BoxCollision->SetBoxExtent(FVector(200.f, 200.f, 200.f));
}

// Called when the game starts or when spawned
void AQuestTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	if (BoxCollision)
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AQuestTriggerBox::OnBoxBeginOverlap);
	}
}

void AQuestTriggerBox::OnBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	ABasePlayer* Player = Cast<ABasePlayer>(OtherActor);
	if (!Player)
	{
		return;
	}

	if (QuestStepToComplete == 5)
	{
		Player->HandleAbandonedBuildingQuest();
	}
	else if (QuestStepToComplete == 6)
	{
		Player->HandleGunQuest();
	}

	Destroy();
}

