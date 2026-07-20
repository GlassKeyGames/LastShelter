#include "Code/Actors/Door.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Code/Actors/BasePlayer.h"

ADoor::ADoor()
{
    PrimaryActorTick.bCanEverTick = false;

    Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
    SetRootComponent(Hinge);

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(Hinge);

    DoorMesh->SetMobility(EComponentMobility::Movable);
    DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    DoorMesh->SetCollisionResponseToAllChannels(ECR_Block);
    DoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

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

    Hinge->SetRelativeRotation(ClosedRotation);

    DoorMesh->SetRelativeLocation(
        DoorBaseRelativeLocation + ClosedOnlyRelativeLocationOffset
    );

    if (InteractBox)
    {
        InteractBox->SetBoxExtent(FVector(300.f, 300.f, 220.f));
        InteractBox->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

        InteractBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        InteractBox->SetCollisionObjectType(ECC_WorldDynamic);
        InteractBox->SetCollisionResponseToAllChannels(ECR_Ignore);
        InteractBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        InteractBox->SetGenerateOverlapEvents(true);

        InteractBox->OnComponentBeginOverlap.AddDynamic(
            this,
            &ADoor::OnInteractBoxBeginOverlap
        );

        InteractBox->OnComponentEndOverlap.AddDynamic(
            this,
            &ADoor::OnInteractBoxEndOverlap
        );
    }
}

void ADoor::ToggleDoor()
{
    if (!Hinge || !DoorMesh)
    {
        return;
    }

    bIsOpen = !bIsOpen;

    if (bIsOpen)
    {
        DoorMesh->SetRelativeLocation(
            DoorBaseRelativeLocation + OpenDoorRelativeLocationOffset
        );

        Hinge->SetRelativeRotation(OpenRotation);

        if (OpenDoorSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                OpenDoorSound,
                GetActorLocation()
            );
        }
    }
    else
    {
        Hinge->SetRelativeRotation(ClosedRotation);

        DoorMesh->SetRelativeLocation(
            DoorBaseRelativeLocation + ClosedOnlyRelativeLocationOffset
        );

        if (CloseDoorSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                CloseDoorSound,
                GetActorLocation()
            );
        }
    }
}

void ADoor::OnInteractBoxBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!Cast<ABasePlayer>(OtherActor))
    {
        return;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("[Door] Player entered door range: %s"),
        *GetName()
    );
}

void ADoor::OnInteractBoxEndOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    if (!Cast<ABasePlayer>(OtherActor))
    {
        return;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("[Door] Player left door range: %s"),
        *GetName()
    );
}