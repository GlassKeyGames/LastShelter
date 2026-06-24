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

    if (!Hinge || !DoorMesh || !DoorMesh->GetStaticMesh())
    {
        UE_LOG(LogTemp, Error, TEXT("ADoor missing Hinge, DoorMesh, or StaticMesh"));
        return;
    }

    DoorMesh->SetHiddenInGame(false);
    DoorMesh->SetVisibility(true, true);

    const FBoxSphereBounds DoorBounds = DoorMesh->GetStaticMesh()->GetBounds();

    // Use HALF the door width, not 2.5x
    const float HalfDoorWidth = DoorBounds.BoxExtent.Y;

    // Put hinge on left side of frame
    AddActorWorldOffset(GetActorRightVector() * HalfDoorWidth);

    // Put door mesh back so its hinge edge lines up with the pivot
    DoorMesh->SetRelativeLocation(FVector(0.f, -HalfDoorWidth, 0.f));

    ClosedRotation = FRotator::ZeroRotator;
    OpenRotation = FRotator(0.f, OpenYawOffset, 0.f);

    UE_LOG(LogTemp, Warning, TEXT("Door setup | HalfDoorWidth=%.2f | DoorRelLoc=%s"),
        HalfDoorWidth,
        *DoorMesh->GetRelativeLocation().ToString());
}

void ADoor::ToggleDoor()
{
    UE_LOG(LogTemp, Warning, TEXT("ToggleDoor called on %s"), *GetName());

    bIsOpen = !bIsOpen;

    if (bIsOpen)
    {
        Hinge->SetRelativeRotation(OpenRotation);

        if (OpenDoorSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, OpenDoorSound, GetActorLocation());
        }
    }
    else
    {
        Hinge->SetRelativeRotation(ClosedRotation);

        if (CloseDoorSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, CloseDoorSound, GetActorLocation());
        }
    }
}