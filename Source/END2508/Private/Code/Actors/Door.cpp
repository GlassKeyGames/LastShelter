#include "Code/Actors/Door.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ADoor::ADoor()
{
    PrimaryActorTick.bCanEverTick = false;

    Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
    SetRootComponent(Hinge);

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(Hinge);

    DoorMesh->SetUsingAbsoluteLocation(false);
    DoorMesh->SetUsingAbsoluteRotation(false);
    DoorMesh->SetUsingAbsoluteScale(false);

    DoorMesh->SetMobility(EComponentMobility::Movable);
    DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    DoorMesh->SetCollisionResponseToAllChannels(ECR_Block);
    DoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    DoorMesh->SetRelativeLocation(FVector::ZeroVector);
    DoorMesh->SetRelativeRotation(FRotator::ZeroRotator);
    DoorMesh->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
}

void ADoor::BeginPlay()
{
    Super::BeginPlay();

    if (!Hinge || !DoorMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("ADoor missing Hinge or DoorMesh"));
        return;
    }

    DoorMesh->SetHiddenInGame(false);
    DoorMesh->SetVisibility(true, true);

    ClosedRotation = ClosedRotationOffset;
    OpenRotation = OpenRotationOffset;

    // Start closed.
    Hinge->SetRelativeRotation(ClosedRotation);
    DoorMesh->SetRelativeLocation(DoorBaseRelativeLocation + ClosedOnlyRelativeLocationOffset);

    UE_LOG(LogTemp, Warning, TEXT("Door Base: %s | Closed Offset: %s | Open Offset: %s"),
        *DoorBaseRelativeLocation.ToString(),
        *ClosedOnlyRelativeLocationOffset.ToString(),
        *OpenDoorRelativeLocationOffset.ToString());
}

void ADoor::ToggleDoor()
{
    UE_LOG(LogTemp, Warning, TEXT("ToggleDoor called on %s"), *GetName());

    if (!Hinge || !DoorMesh)
    {
        return;
    }

    bIsOpen = !bIsOpen;

    if (bIsOpen)
    {
        // OPEN DOES NOT USE CLOSED OFFSET.
        DoorMesh->SetRelativeLocation(DoorBaseRelativeLocation + OpenDoorRelativeLocationOffset);
        Hinge->SetRelativeRotation(OpenRotation);

        if (OpenDoorSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, OpenDoorSound, GetActorLocation());
        }
    }
    else
    {
        // CLOSED USES THE CLOSED-ONLY OFFSET.
        Hinge->SetRelativeRotation(ClosedRotation);
        DoorMesh->SetRelativeLocation(DoorBaseRelativeLocation + ClosedOnlyRelativeLocationOffset);

        if (CloseDoorSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, CloseDoorSound, GetActorLocation());
        }
    }
}