// Fill out your copyright notice in the Description page of Project Settings.


#include "Code/Actors/Projectile.h"
#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../END2508.h"
#include "Code/Utility/CodeInterface.h"
#include "Code/Actors/Agent.h"
#include "Code/Actors/BarrelSpawner.h"

// Sets default values
AProjectile::AProjectile() : Size(.18f, .18f, .18f)
{
    PrimaryActorTick.bCanEverTick = false;          

    SphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
    SetRootComponent(SphereCollider);

    SphereCollider->InitSphereRadius(6.f);
    SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereCollider->SetCollisionObjectType(ECC_WorldDynamic);
    SphereCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereCollider->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    SphereCollider->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    SphereCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

    SphereCollider->SetNotifyRigidBodyCollision(true);
    SphereCollider->SetGenerateOverlapEvents(false); // IMPORTANT: hit-only

    // Visual
    SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>("SphereMesh");
    SphereMesh->SetupAttachment(SphereCollider);
    SphereMesh->SetCollisionProfileName(TEXT("NoCollision"));
    SphereMesh->SetRelativeScale3D(FVector(0.1f));
    SphereCollider->InitSphereRadius(3.f);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Asset(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (Asset.Succeeded()) SphereMesh->SetStaticMesh(Asset.Object);

    // Bind events
    SphereCollider->OnComponentHit.AddDynamic(this, &AProjectile::HandleHit);
    SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::HandleOverlap);

    // Movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->InitialSpeed = 4000.f;
    ProjectileMovement->MaxSpeed = 4000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.f;
    ProjectileMovement->bShouldBounce = false;

    InitialLifeSpan = 3.f;                          
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* MyOwner = GetOwner())
    {
        SphereCollider->IgnoreActorWhenMoving(MyOwner, true);
    }

    if (APawn* Inst = GetInstigator())
    {
        SphereCollider->IgnoreActorWhenMoving(Inst, true);
    }
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::HandleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    const AActor* MyOwner = GetOwner();
    const APawn* MyInstigator = GetInstigator();

    if (OtherActor == MyOwner || OtherActor == MyInstigator ||
        (MyOwner && OtherActor->GetOwner() == MyOwner))
    {
        return;
    }


    AController* InstigatorController = nullptr;
    if (const APawn* InstPawn = Cast<APawn>(MyInstigator))
    {
        InstigatorController = InstPawn->GetController();
    }

    const bool bHitAgent = Cast<AAgent>(OtherActor) != nullptr;
    const bool bHitSpawner = Cast<ABarrelSpawner>(OtherActor) != nullptr;

    if (!bHitAgent && !bHitSpawner)
    {
        Destroy();
        return;
    }

    UGameplayStatics::ApplyDamage(
        OtherActor,
        25.f,
        InstigatorController,
        this,
        UDamageType::StaticClass()
    );

    Destroy();
}

void AProjectile::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{

	Destroy();
}

