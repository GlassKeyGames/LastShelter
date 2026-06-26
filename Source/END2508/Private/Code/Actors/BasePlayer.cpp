// Fill out your copyright notice in the Description page of Project Settings.

#include "Code/Actors/BasePlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Code/Actors/Rifle.h"
#include "Both/CharacterAnimation.h"
#include "Both/PlayerHUD.h"
#include "EngineUtils.h"
#include "Code/Actors/BarrelSpawner.h"
#include "Components/ProgressBar.h"
#include "Code/Utility/RegenHealthComponent.h"
#include "Code/Utility/HealthComponent.h"
#include "Perception/AISense_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Code/Actors/MyGameMode.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "BuildComponent.h" 
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Code/Actors/PickUps.h"
#include "Code/Actors/Door.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"

ABasePlayer::ABasePlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<URegenHealthComponent>(TEXT("HealthComponent")))
{
    // --- Spring Arm ---
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->TargetArmLength = 290.f;
    SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bInheritPitch = true;
    SpringArm->bInheritYaw = true;
    SpringArm->bInheritRoll = true;
    SpringArm->bDoCollisionTest = false;

    // --- Camera ---
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;
    Camera->SetRelativeLocation(FVector::ZeroVector);


   static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(TEXT("/Game/Both/DoNotConvert/ABP_CharacterAnimation.ABP_CharacterAnimation_C"));
   if (AnimBPClass.Succeeded())
   {
       GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
   }

    //Build Component
    BuildComponent = CreateDefaultSubobject<UBuildComponent>(TEXT("BuildComponent"));

    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

    Hotbar.SetNum(4);
    for (FHotbarEntry& Entry : Hotbar)
    {
        Entry.bOccupied = false;
        Entry.ItemClass = nullptr;
        Entry.Icon = nullptr;
        Entry.EquipType = EEquipState::Unarmed;
    }
    ActiveHotbarIndex = 0;
    EquipState = EEquipState::Unarmed;
}

void ABasePlayer::ToggleCameraView()
{
    if (!SpringArm || !Camera)
        return;

    bIsFirstPerson = !bIsFirstPerson;

    if (bIsFirstPerson)
    {
        // First Person
        SpringArm->TargetArmLength = 0.f;
        SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
        Camera->SetRelativeLocation(FVector(40.f, 0.f, 0.f));

        bUseControllerRotationPitch = true;
        bUseControllerRotationYaw = true;
        bUseControllerRotationRoll = false;


        UE_LOG(LogTemp, Warning, TEXT("Switched to FIRST PERSON"));
    }
    else
    {
        // Third Person
        SpringArm->TargetArmLength = 290.f;
        SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));

        // Rotation behavior
        bUseControllerRotationPitch = false;
        bUseControllerRotationYaw = false;
        bUseControllerRotationRoll = false;

        // Show body again
        GetMesh()->SetOwnerNoSee(false);

        UE_LOG(LogTemp, Warning, TEXT("Switched to THIRD PERSON"));
    }
}

void ABasePlayer::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (MappingContext)
            {
                Subsystem->AddMappingContext(MappingContext, 0);
                UE_LOG(LogTemp, Warning, TEXT("? Added Input Mapping Context"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("? InputMappingContext is NULL. Assign it in BP."));
            }
        }
    }

    Tags.Add(FName("Player"));


    if (!Axe && AxeClass)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        Axe = GetWorld()->SpawnActor<AAxe>(AxeClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
        if (Axe)
        {
            Axe->InitializeAxe(this);

            if (!GetMesh()->DoesSocketExist(AxeAttachSocket))
            {
                UE_LOG(LogTemp, Error, TEXT("Missing axe socket: %s"), *AxeAttachSocket.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Axe socket found: %s"), *AxeAttachSocket.ToString());
            }

            Axe->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AxeAttachSocket);
            Axe->SetActorRelativeLocation(FVector::ZeroVector);
            Axe->SetActorRelativeRotation(FRotator::ZeroRotator);

            Axe->SetActorHiddenInGame(true);
        }
    }

    if (!Pickaxe && PickaxeClass)
    {
        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.Instigator = this;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        Pickaxe = GetWorld()->SpawnActor<APickaxe>(PickaxeClass, GetActorLocation(), GetActorRotation(), Params);
        if (Pickaxe)
        {
            Pickaxe->InitializeAxe(this); // inherited behavior if APickaxe derives from AAxe

            // Attach to SAME socket as axe
            Pickaxe->AttachToComponent(
                GetMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                AxeAttachSocket
            );

            Pickaxe->SetActorRelativeLocation(FVector::ZeroVector);
            Pickaxe->SetActorRelativeRotation(FRotator::ZeroRotator);

            Pickaxe->SetActorHiddenInGame(true); // start hidden until equipped
            Pickaxe->SetActorEnableCollision(false);

            UE_LOG(LogTemp, Warning, TEXT("Pickaxe attached to %s"), *AxeAttachSocket.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn Pickaxe"));
        }
    }

    if (!Flashlight && FlashlightClass)
    {
        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.Instigator = this;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        Flashlight = GetWorld()->SpawnActor<AFlashlight>(
            FlashlightClass, GetActorLocation(), GetActorRotation(), Params);

        if (Flashlight)
        {
            Flashlight->AttachToComponent(
                GetMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                FlashlightAttachSocket
            );

            Flashlight->SetActorRelativeLocation(FVector::ZeroVector);
            Flashlight->SetActorRelativeRotation(FRotator::ZeroRotator);

            // hidden until equipped
            Flashlight->SetActorHiddenInGame(true);
            Flashlight->SetActorEnableCollision(false);

            UE_LOG(LogTemp, Warning, TEXT("[Flashlight] Spawned and attached to %s"),
                *FlashlightAttachSocket.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Flashlight] Failed to spawn Flashlight"));
        }
    }

    if (StimuliSource)
    {
        StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
        StimuliSource->RegisterWithPerceptionSystem();
    }

    if (HUDWidgetClass)
    {
        HUDWidget = CreateWidget<UPlayerHUD>(GetWorld(), HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();

            HUDWidget->SetInteractTextVisible(false, FText::GetEmpty());

            HealthComponent = FindComponentByClass<UHealthComponent>();
            if (HealthComponent)
            {
                HealthComponent->ResetHealth(); 
                UpdateHealthHUD(HealthComponent->CurrentHealth, HealthComponent->MaxHealth);

                HealthComponent->OnHeal.AddDynamic(HUDWidget, &UPlayerHUD::SetHealth);
                HealthComponent->OnHurt.AddDynamic(HUDWidget, &UPlayerHUD::SetHealth);
            }

            UE_LOG(LogTemp, Warning, TEXT("Player HUD created successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create Player HUD"));
        }
    }

    if (HUDWidget)
    {
        HUDWidget->SetInventorySlotIcon(0, nullptr);
        HUDWidget->SetInventorySlotIcon(1, nullptr);
        HUDWidget->SetInventorySlotIcon(2, nullptr);
        HUDWidget->SetInventorySlotIcon(3, nullptr);

        HUDWidget->SetActiveInventorySlot(0);
        HUDWidget->SetInteractTextVisible(false, FText::GetEmpty());
        UpdateQuestText();
    }


    if (HUDWidget)
    {
        WoodCount = 0;
        StoneCount = 0;
        HUDWidget->SetResources(WoodCount, StoneCount);
    }

    PlayerRifle = Cast<ARifle>(Weapon);
    if (HUDWidget && PlayerRifle)
    {
        PlayerRifle->OnAmmoChanged.AddDynamic(HUDWidget, &UPlayerHUD::SetAmmo);
        UpdateAmmoHUD();
        UE_LOG(LogTemp, Warning, TEXT("[Rifle] HUD bound to rifle ammo"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Rifle] HUD bind skipped. HUD=%p PlayerRifle=%p Weapon=%p"), HUDWidget, PlayerRifle, Weapon);
    }

    if (AgentToSpawn && HasAuthority())
    {
        const FVector SpawnLoc = GetActorLocation() + FVector(300.f, 0.f, 100.f);
        const FRotator SpawnRot = GetActorRotation();

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        if (APawn* Spawned = GetWorld()->SpawnActor<APawn>(AgentToSpawn, SpawnLoc, SpawnRot, Params))
        {
            Spawned->SpawnDefaultController();
            UE_LOG(LogTemp, Log, TEXT("Spawned AI Pawn"), *Spawned->GetName());
        }
    }

    if (WeaponChild)
    {
        WeaponChild->SetHiddenInGame(true, true);
        WeaponChild->SetVisibility(false, true);
    }

    if (Weapon)
    {
        Weapon->SetActorHiddenInGame(true);
    }

    EquipState = EEquipState::Unarmed;
    EquipHotbarSlot(0);

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
        UE_LOG(LogTemp, Warning, TEXT("Input mode set to GameOnly"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController not found!"));
    }

    if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
    {
        Anim->OnDeathEnded.AddDynamic(this, &ABasePlayer::PlayerLost);
        UE_LOG(LogTemp, Warning, TEXT("Bound PlayerLost to OnDeathEnded"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Could not bind PlayerLost: Anim instance not valid"));
    }

    if (BuildComponent)
    {
        BuildComponent->Camera = Camera;
    }

    if (BuildComponent)
    {
        if (BuildComponent->FenceMesh)
        {
            UE_LOG(LogTemp, Warning, TEXT("[BasePlayer] FenceMesh on BuildComponent = %s"),
                *BuildComponent->FenceMesh->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[BasePlayer] FenceMesh is NOT set on BuildComponent!"));
        }
    }

    if (SpringArm)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CAM-BEGIN] ArmLen=%.1f Rel=%s SockOff=%s TgtOff=%s DoColl=%d UsePCR=%d"),
            SpringArm->TargetArmLength,
            *SpringArm->GetRelativeLocation().ToString(),
            *SpringArm->SocketOffset.ToString(),
            *SpringArm->TargetOffset.ToString(),
            SpringArm->bDoCollisionTest ? 1 : 0,
            SpringArm->bUsePawnControlRotation ? 1 : 0
        );
    }
    if (Camera)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CAM-BEGIN] CamRel=%s UsePCR=%d"),
            *Camera->GetRelativeLocation().ToString(),
            Camera->bUsePawnControlRotation ? 1 : 0
        );
    }
}

void ABasePlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (SpringArm && Camera)
    {
        const FVector ArmLoc = SpringArm->GetComponentLocation();
        const FVector CamLoc = Camera->GetComponentLocation();
        const float Dist = FVector::Dist(ArmLoc, CamLoc);

        UE_LOG(LogTemp, Warning,
            TEXT("[CAM] ArmLen=%.1f Dist=%.1f ArmRel=%s SockOff=%s TgtOff=%s CamRel=%s"),
            SpringArm->TargetArmLength,
            Dist,
            *SpringArm->GetRelativeLocation().ToString(),
            *SpringArm->SocketOffset.ToString(),
            *SpringArm->TargetOffset.ToString(),
            *Camera->GetRelativeLocation().ToString()
        );
    } 


    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC || !HUDWidget) return;

    FVector Start;
    FRotator Rotation;
    PC->GetPlayerViewPoint(Start, Rotation);

    FVector End = Start + Rotation.Vector() * 5000.f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this); // Don't hit self

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    bool bShouldHighlight = false;

    if (bHit && Hit.GetActor())
    {
        AActor* HitActor = Hit.GetActor();

        UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s | HasEnemyTag=%d"),
            *HitActor->GetName(),
            HitActor->ActorHasTag("Enemy") ? 1 : 0);

        bShouldHighlight = HitActor->ActorHasTag("Enemy");
    }

   
    bool bInBuildOrDestroy = false;
    if (BuildComponent)
    {
        bInBuildOrDestroy =
            BuildComponent->IsBuildModeActive() ||
            BuildComponent->IsDestroyModeActive();
    }

    

    HUDWidget->SetReticleColor(bShouldHighlight);

}







// MOVEMENT & LOOK INPUT

void ABasePlayer::Move(const FInputActionValue& Value)
{
    const FVector2D Input = Value.Get<FVector2D>();
    if (!Controller) return;

    const FRotator ControlRot = Controller->GetControlRotation();
    const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

    const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDir, Input.Y); //  W/S
    AddMovementInput(RightDir, Input.X); //  A/D
    UE_LOG(LogTemp, Warning, TEXT("Move Input: X=%f Y=%f"), Input.X, Input.Y);

}

void ABasePlayer::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (!Controller) return;

    AddControllerYawInput(LookAxisVector.X);

    float NewPitch = GetControlRotation().Pitch - LookAxisVector.Y;

    const float MinPitch = bIsFirstPerson ? -25.f : -60.f;
    const float MaxPitch = bIsFirstPerson ? 50.f : 35.f;

    NewPitch = FMath::ClampAngle(NewPitch, MinPitch, MaxPitch);

    FRotator NewRot = GetControlRotation();
    NewRot.Pitch = NewPitch;
    Controller->SetControlRotation(NewRot);
}

void ABasePlayer::Jump()
{
    if (!CanJump())
    {
        UE_LOG(LogTemp, Error, TEXT("? Can't Jump!"));
        return;
    }

    Super::Jump();
    bJustJumped = true;
    UE_LOG(LogTemp, Warning, TEXT("? Jump pressed!"));
}

void ABasePlayer::StopJumping()
{
    Super::StopJumping();
    UE_LOG(LogTemp, Warning, TEXT("Jump released!"));
}

void ABasePlayer::InputAxisMoveForward(float Value)
{
    FRotator ReturnValue = GetControlRotation();
    float Yaw = ReturnValue.Yaw;
    FRotator MakeRotator(0.f, Yaw, 0.f);
    FVector WorldDirection = FRotator(0.f, Yaw, 0.f).Vector();
    AddMovementInput(WorldDirection, Value);
}

void ABasePlayer::InputAxisMoveSideways(float Value)
{
    FRotator ControlRot(0.f, GetControlRotation().Yaw, 0.f);
    FVector Right = ControlRot.RotateVector(FVector::RightVector);
    AddMovementInput(Right, Value);
}

void ABasePlayer::InputAxisLookUp(float Value)
{
    if (!Controller) return;

    float NewPitch = GetControlRotation().Pitch - Value;

    // Different limits for 1st vs 3rd person
    const float MinPitch = bIsFirstPerson ? -70.f : -80.f; // up
    const float MaxPitch = bIsFirstPerson ? 55.f : 80.f; // down

    NewPitch = FMath::ClampAngle(NewPitch, MinPitch, MaxPitch);

    FRotator NewRot = GetControlRotation();
    NewRot.Pitch = NewPitch;

    Controller->SetControlRotation(NewRot);
}









// BINDING INPUTS

void ABasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

	// THIS IS THE OLD INPUT SYSTEM - COMMENTED OUT FOR ENHANCED INPUT
    //PlayerInputComponent->BindAxis("MoveForward", this, &ABasePlayer::InputAxisMoveForward);
    //PlayerInputComponent->BindAxis("MoveRight", this, &ABasePlayer::InputAxisMoveSideways);
    //PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &ABasePlayer::AddControllerYawInput);
    //PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABasePlayer::InputAxisLookUp);
    //PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ABasePlayer::AttackInput);
    //PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABasePlayer::HandleReloadInput);
    
  
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayer::Move);
        }

        if (LookAction)
        {
            EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayer::Look);
        }

        if (AttackAction)
        {
            EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ABasePlayer::AttackInput);
        }

        if (ReloadAction)
        {
            EIC->BindAction(ReloadAction, ETriggerEvent::Started, this, &ABasePlayer::HandleReloadInput);
        }

        if (BuildModeAction)
        {
            EIC->BindAction(BuildModeAction,
                ETriggerEvent::Started,
                this,
                &ABasePlayer::HandleBuildKey);

            UE_LOG(LogTemp, Warning, TEXT("Bound IA_BuildMode to HandleBuildKey"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BuildModeAction is NULL (assign IA_BuildMode in BP defaults)"));
        }

        if (InteractAction)
        {
            EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &ABasePlayer::OnInteract);
        }

        if (ToggleViewAction)
        {
            EIC->BindAction(
                ToggleViewAction,
                ETriggerEvent::Started,
                this,
                &ABasePlayer::ToggleCameraView
            );

            UE_LOG(LogTemp, Warning, TEXT("Bound IA_ToggleView to ToggleCameraView"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ToggleViewAction is NULL (assign IA_ToggleView in BP defaults)"));
        }

		// Hotbar bindings
        if (Hotbar1Action)
            EIC->BindAction(Hotbar1Action, ETriggerEvent::Started, this, &ABasePlayer::Hotbar1Pressed);

        if (Hotbar2Action)
            EIC->BindAction(Hotbar2Action, ETriggerEvent::Started, this, &ABasePlayer::Hotbar2Pressed);

        if (Hotbar3Action)
            EIC->BindAction(Hotbar3Action, ETriggerEvent::Started, this, &ABasePlayer::Hotbar3Pressed);

        if (Hotbar4Action)
            EIC->BindAction(Hotbar4Action, ETriggerEvent::Started, this, &ABasePlayer::Hotbar4Pressed);

        if (JumpAction)
        {
            UE_LOG(LogTemp, Warning, TEXT("? Binding Jump Action"));
            EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ABasePlayer::Jump);
            EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABasePlayer::StopJumping);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("? JumpAction is null — assign IA_Jump in Blueprint"));
        }

        UE_LOG(LogTemp, Warning, TEXT("? JumpAction is bound to Spacebar"));
    }
}









// WEAPONS

void ABasePlayer::DoAxeAttack()
{
    if (!Axe || !bCanAxeAttack) return;

    bCanAxeAttack = false;
    GetWorldTimerManager().SetTimer(
        AxeCooldownHandle,
        [this]() { bCanAxeAttack = true; },
        AxeAttackCooldown,
        false
    );

    if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
    {
        Anim->AxeAnimation();  // plays chop visual
    }

    // Simple: damage now
    Axe->Attack();
}

void ABasePlayer::OnAxeHitNotify()
{
    if (EquipState == EEquipState::Axe && Axe && !bIsDead)
    {
        Axe->Attack();
    }
}

void ABasePlayer::DoPickaxeAttack()
{
    if (!Pickaxe || bIsDead) return;

    if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
    {
        Anim->PickaxeAnimation();   // <- your new pickaxe anim
    }

    // Simple immediate hit:
    Pickaxe->Attack();
}

void ABasePlayer::ToggleFlashlight()
{
    if (!Flashlight) return;

    // If your flashlight actor has ToggleLight():
    Flashlight->ToggleLight();
}

void ABasePlayer::DoFlashlightUse()
{
    if (!Flashlight || bIsDead) return;

    // Optional animation
    if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
    {
        Anim->FlashlightAnimation();
    }

    // Toggle light
    Flashlight->ToggleLight();
}

void ABasePlayer::HandleReloadInput()
{
    if (!Weapon) return;

    Weapon->ReloadAmmo();

    if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
    {
        Anim->ReloadAnimation();

        FTimerHandle FallbackHandle;
        GetWorldTimerManager().SetTimer(FallbackHandle, [this]()
            {
                if (Weapon)
                {
                    Weapon->CompleteReload();
                    Weapon->ResetAction();
                }
            }, 1.5f, false);
    }
}









// RESOURCES

void ABasePlayer::AddWood(int32 Amount)
{
    WoodCount = FMath::Max(0, WoodCount + Amount);
    UpdateResourceHUD();
    HandleWoodQuest();
}

void ABasePlayer::AddStone(int32 Amount)
{
    StoneCount = FMath::Max(0, StoneCount + Amount);
    UpdateResourceHUD();
    HandleStoneQuest();
}









// HUD

void ABasePlayer::UpdateResourceHUD()
{
    if (HUDWidget)
    {
        HUDWidget->SetResources(WoodCount, StoneCount);
    }
}

void ABasePlayer::UpdateHealthHUD(float Current, float Max)
{
    if (!HUDWidget)
        return;

    UProgressBar* HealthBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HealthBar")));
    if (!HealthBar)
        return;

    float Percent = (Max > 0.f) ? (Current / Max) : 0.f;
    HealthBar->SetPercent(Percent);
}

void ABasePlayer::UpdateAmmoHUD()
{
    if (HUDWidget && PlayerRifle)
    {
        HUDWidget->SetAmmo(PlayerRifle->GetCurrentAmmo(), PlayerRifle->GetMaxAmmo());
    }
}

void ABasePlayer::AddMaxAmmo(int32 Amount)
{
    if (!PlayerRifle) return;

    float NewMaxAmmo = PlayerRifle->GetMaxAmmo() + Amount;
    PlayerRifle->SetMaxAmmo(NewMaxAmmo);

    UpdateAmmoHUD();
}







// PLAYER ATTACK & INTERACTION

void ABasePlayer::AttackInput()
{
    if (bIsDead) return;

    if (EquipState == EEquipState::Axe)
    {
        DoAxeAttack();
        return;
    }

    if (EquipState == EEquipState::PickAxe)
    {
        DoPickaxeAttack();
        return;
    }

    if (EquipState == EEquipState::Flashlight)
    {
        DoFlashlightUse();   // same button toggles light / plays anim
        return;
    }

    if (EquipState == EEquipState::Rifle)
    {
        if (IsPlayerControlled() && Weapon)
        {
            UE_LOG(LogTemp, Warning, TEXT("Player attempted to fire"));

            bool bFired = Weapon->Attack();

            if (!bFired)
            {
                UE_LOG(LogTemp, Warning, TEXT("Weapon can't fire. Ammo: %.0f / %.0f"),
                    Weapon->GetCurrentAmmo(), Weapon->GetMaxAmmo());

                if (!Weapon->IsActionHappening() && Weapon->GetCurrentAmmo() <= 0)
                {
                    Weapon->ReloadAmmo();
                    if (UCharacterAnimation* Anim = Cast<UCharacterAnimation>(GetMesh()->GetAnimInstance()))
                    {
                        Anim->ReloadAnimation();
                    }
                }
            }
        }
    }
}

// AGENT ATTACK
void ABasePlayer::TellAllAgentsToAttack()
{
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Actor = *It;

        if (!IsValid(Actor)) continue;

        if (Actor->GetClass()->ImplementsInterface(UCodeInterface::StaticClass()))
        {
            ICodeInterface* Interface = Cast<ICodeInterface>(Actor);
            if (Interface)
            {
                UE_LOG(LogTemp, Warning, TEXT("Player telling %s to attack via interface!"), *Actor->GetName());
                Interface->EnemyAttack(); 
            }
        }
    }
}









// WIN / LOSS CONDITION

void ABasePlayer::PlayerLost()
{
    UE_LOG(LogTemp, Warning, TEXT("PlayerLost triggered — player death animation finished"));

    // Broadcast for any listeners 
    OnPlayerLost.Broadcast();

    // Remove HUD
    if (HUDWidget)
    {
        HUDWidget->RemoveFromParent();
        HUDWidget = nullptr;
    }

    // Enable mouse cursor
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->bShowMouseCursor = true;
    }

    UE_LOG(LogTemp, Warning, TEXT("You Lose screen triggered from PlayerLost"));
}

void ABasePlayer::PlayerWin()
{
    UE_LOG(LogTemp, Warning, TEXT("PlayerWin triggered — player has won"));

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        DisableInput(PC);
    }

    if (HUDWidget)
    {
        HUDWidget->RemoveFromParent();
        HUDWidget = nullptr;
    }

    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));
    if (!GM || !GM->ResultsWidgetObject)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode or ResultsWidgetObject is null in PlayerWin()"));
        return;
    }

    GM->ResultsWidgetObject->AddToViewport();
    GM->ResultsWidgetObject->SetWin();

    if (PC)
    {
        PC->SetInputMode(FInputModeUIOnly());
        PC->bShowMouseCursor = true;
    }

    UE_LOG(LogTemp, Warning, TEXT("You Win screen shown from PlayerWin"));
}








// INTERACTION
void ABasePlayer::OnInteract()
{
    UE_LOG(LogTemp, Warning, TEXT("OnInteract pressed"));

    ADoor* FoundDoor = nullptr;

    const FVector Start = GetActorLocation() + FVector(0.f, 0.f, 50.f);
    const FVector Center = Start + GetActorForwardVector() * 150.f;
    const float Radius = 120.f;

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

    const bool bHit = GetWorld()->OverlapMultiByObjectType(
        Overlaps,
        Center,
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeSphere(Radius),
        Params
    );

    if (bHit)
    {
        for (const FOverlapResult& Result : Overlaps)
        {
            AActor* HitActor = nullptr;

            if (Result.Component.IsValid())
            {
                HitActor = Result.Component->GetOwner();
            }

            if (HitActor)
            {
                UE_LOG(LogTemp, Warning, TEXT("Overlap hit: %s"), *HitActor->GetName());

                if (ADoor* Door = Cast<ADoor>(HitActor))
                {
                    FoundDoor = Door;
                    break;
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No overlap found"));
    }

    if (FoundDoor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Found nearby door: %s"), *FoundDoor->GetName());

        FoundDoor->ToggleDoor();

        if (HUDWidget)
        {
            HUDWidget->SetInteractTextVisible(
                true,
                FoundDoor->IsOpen()
                ? FText::FromString(TEXT("Press E to Close"))
                : FText::FromString(TEXT("Press E to Open"))
            );
        }

        return;
    }

    if (CurrentPickup)
    {
        if (CurrentPickup->TryPickup(this))
        {
            CurrentPickup = nullptr;
            return;
        }
    }

    if (BuildComponent)
    {
        if (BuildComponent->IsDestroyModeActive())
        {
            BuildComponent->DestroyCurrentTarget();
        }
        else
        {
            BuildComponent->PlaceBuildable();
        }
    }
}





// OPEN BUILD MENU
void ABasePlayer::HandleBuildKey()
{
    if (!BuildComponent)
        return;

    BuildComponent->OpenBuildMenu();
}









// HOTBAR & EQUIPMENT SLOTS
bool ABasePlayer::AddToFirstAvailableSlot(TSubclassOf<AActor> ItemClass, UTexture2D* Icon, EEquipState EquipType, int32& OutIndex)
{
    OutIndex = INDEX_NONE;

    if (!ItemClass || EquipType == EEquipState::Unarmed)
    {
        return false;
    }

    // Slot 4 (index 3) is reserved for guns
    if (EquipType == EEquipState::Rifle)
    {
        const int32 GunSlotIndex = 3;

        if (Hotbar.IsValidIndex(GunSlotIndex) && !Hotbar[GunSlotIndex].bOccupied)
        {
            Hotbar[GunSlotIndex].bOccupied = true;
            Hotbar[GunSlotIndex].ItemClass = ItemClass;
            Hotbar[GunSlotIndex].Icon = Icon;
            Hotbar[GunSlotIndex].EquipType = EquipType;

            if (HUDWidget)
            {
                HUDWidget->SetInventorySlotIcon(GunSlotIndex, Icon);
            }

            OutIndex = GunSlotIndex;
            EquipHotbarSlot(GunSlotIndex);
            UE_LOG(LogTemp, Warning, TEXT("Picked up Rifle into reserved slot %d"), GunSlotIndex + 1);
            return true;
        }

        UE_LOG(LogTemp, Warning, TEXT("Gun slot is already occupied"));
        return false;
    }

    // Non-gun items can only go into slots 1-3 (indices 0-2)
    for (int32 i = 0; i < 3; ++i)
    {
        if (!Hotbar[i].bOccupied)
        {
            Hotbar[i].bOccupied = true;
            Hotbar[i].ItemClass = ItemClass;
            Hotbar[i].Icon = Icon;
            Hotbar[i].EquipType = EquipType;

            if (HUDWidget)
            {
                HUDWidget->SetInventorySlotIcon(i, Icon);
            }

            OutIndex = i;
            UE_LOG(LogTemp, Warning, TEXT("Picked up %d into slot %d"), (int32)EquipType, i + 1);
            return true;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("No available non-gun slots"));
    return false;
}

void ABasePlayer::Hotbar1Pressed(const FInputActionValue& Value)
{
    EquipHotbarSlot(0);   // slot index 0 => key "1"
}

void ABasePlayer::Hotbar2Pressed(const FInputActionValue& Value)
{
    EquipHotbarSlot(1);
}

void ABasePlayer::Hotbar3Pressed(const FInputActionValue& Value)
{
    EquipHotbarSlot(2);
}

void ABasePlayer::Hotbar4Pressed(const FInputActionValue& Value)
{
    EquipHotbarSlot(3);
}


void ABasePlayer::EquipHotbarSlot(int32 Index)
{
    if (Index < 0 || Index >= Hotbar.Num()) return;

    ActiveHotbarIndex = Index;

    // Empty slot -> unarmed
    if (!Hotbar[Index].bOccupied)
    {
        EquipState = EEquipState::Unarmed;
    }
    else
    {
        EquipState = Hotbar[Index].EquipType;
    }

    const bool bAxe = (EquipState == EEquipState::Axe);
    const bool bRifle = (EquipState == EEquipState::Rifle);
    const bool bPickaxe = (EquipState == EEquipState::PickAxe);
    const bool bFlashlight = (EquipState == EEquipState::Flashlight);

    if (Axe)
    {
        Axe->SetActorHiddenInGame(!bAxe);

        if (bAxe && GetMesh())
        {
            Axe->AttachToComponent(
                GetMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                AxeAttachSocket
            );

            Axe->SetActorRelativeLocation(FVector::ZeroVector);
            Axe->SetActorRelativeRotation(FRotator::ZeroRotator);
            Axe->SetActorEnableCollision(false);
        }
    }

    if (Pickaxe)
    {
        Pickaxe->SetActorHiddenInGame(!bPickaxe);

        if (bPickaxe && GetMesh())
        {
            Pickaxe->AttachToComponent(
                GetMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                AxeAttachSocket
            );

            Pickaxe->SetActorRelativeLocation(FVector::ZeroVector);
            Pickaxe->SetActorRelativeRotation(FRotator::ZeroRotator);
            Pickaxe->SetActorEnableCollision(false);
        }
    }

    if (WeaponChild)
    {
        WeaponChild->SetHiddenInGame(!bRifle, true);
        WeaponChild->SetVisibility(bRifle, true);
    }

    if (Weapon)
    {
        Weapon->SetActorHiddenInGame(!bRifle);
        if (bRifle)
        {
            Weapon->AttachToComponent(GetMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                WeaponAttachSocket);
            Weapon->SetActorRelativeLocation(FVector::ZeroVector);
            Weapon->SetActorRelativeRotation(FRotator::ZeroRotator);
        }
    }

    if (Flashlight)
    {
        Flashlight->SetActorHiddenInGame(!bFlashlight);
        if (!bFlashlight)
        {
            Flashlight->SetLightEnabled(false);
        }
    }

    if (HUDWidget)
    {
        HUDWidget->SetActiveInventorySlot(Index);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Equip] Slot=%d State=%d"), Index, (int32)EquipState);
}







// PICKUP INTERACTION
void ABasePlayer::SetCurrentPickup(APickUps* NewPickup)
{
    CurrentPickup = NewPickup;
}


void ABasePlayer::UpdateQuestText()
{
    if (!HUDWidget) return;

    switch (QuestStep)
    {
    case 0:
        HUDWidget->SetObjectiveText(FText::FromString(TEXT("Find the Axe.")));
        break;

    case 1:
        HUDWidget->SetObjectiveText(FText::FromString(TEXT("Use the Axe on Trees for Wood.")));
        break;

    case 2:
        HUDWidget->SetObjectiveText(FText::FromString(TEXT("Find the Pickaxe.")));
        break;

    case 3:
        HUDWidget->SetObjectiveText(FText::FromString(TEXT("Use the Pickaxe on Stone.")));
        break;

    case 4:
        HUDWidget->SetObjectiveText(FText::FromString(TEXT("Use resources to build a shelter for protection.")));
        break;

    case 5:
        HUDWidget->SetObjectiveText(FText::FromString(TEXT("Find abandon shelter at the edge of the woods.")));
        break;

    case 6:
        HUDWidget->SetObjectiveText(FText::FromString(TEXT("Search for Gun.")));
        break;

    case 7:
        HUDWidget->SetObjectiveText(FText::FromString(TEXT("Search for Ammo.")));
        break;

    case 8:
        HUDWidget->SetObjectiveText(FText::FromString(TEXT("Survive the final wave: destroy all zombie spawners and kill all remaining zombies.")));
        break;

    default:
        break;
    }
}

void ABasePlayer::AdvanceQuestStep()
{
    QuestStep++;
    UpdateQuestText();
}

void ABasePlayer::HandlePickupQuest(EEquipState PickedUpType)
{
    if (QuestStep == 0 && PickedUpType == EEquipState::Axe)
    {
        AdvanceQuestStep();
    }
    else if (QuestStep == 2 && PickedUpType == EEquipState::PickAxe)
    {
        AdvanceQuestStep();
    }
    else if (QuestStep == 6 && PickedUpType == EEquipState::Rifle)
    {
        HandleGunQuest();
    }
}

void ABasePlayer::HandleWoodQuest()
{
    if (QuestStep == 1 && WoodCount > 0)
    {
        AdvanceQuestStep();
    }
}

void ABasePlayer::HandleStoneQuest()
{
    if (QuestStep == 3 && StoneCount > 0)
    {
        AdvanceQuestStep();
    }
}

void ABasePlayer::HandleBuildQuest()
{
    if (QuestStep == 4)
    {
        AdvanceQuestStep();
    }
}

void ABasePlayer::RegisterPlacedBuildByType(bool bFoundation, bool bWall, bool bDoorFrame, bool bCeiling)
{
    if (QuestStep == 4)
    {
        if (bFoundation || bWall || bDoorFrame || bCeiling)
        {
            HandleBuildQuest();
        }
    }
}

void ABasePlayer::HandleAbandonedBuildingQuest()
{
    if (QuestStep == 5)
    {
        AdvanceQuestStep();
    }
}

void ABasePlayer::HandleGunQuest()
{
    if (QuestStep == 6)
    {
        AdvanceQuestStep();
    }
}

void ABasePlayer::HandleAmmoQuest()
{
    if (QuestStep == 7)
    {
        AdvanceQuestStep();
    }
}

void ABasePlayer::ForceFirstPerson(bool bEnable)
{
    if (!SpringArm || !Camera)
        return;

    bIsFirstPerson = bEnable;

    if (bIsFirstPerson)
    {
        SpringArm->TargetArmLength = 0.f;
        SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
        Camera->SetRelativeLocation(FVector(40.f, 0.f, 0.f));

        bUseControllerRotationPitch = true;
        bUseControllerRotationYaw = true;
        bUseControllerRotationRoll = false;

        GetMesh()->SetOwnerNoSee(true);
    }
    else
    {
        SpringArm->TargetArmLength = 290.f;
        SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
        Camera->SetRelativeLocation(FVector::ZeroVector);

        bUseControllerRotationPitch = false;
        bUseControllerRotationYaw = false;
        bUseControllerRotationRoll = false;

        GetMesh()->SetOwnerNoSee(false);
    }
}
