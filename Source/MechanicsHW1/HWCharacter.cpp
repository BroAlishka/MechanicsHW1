#include "HWCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Engine/Engine.h"

AHWCharacter::AHWCharacter() {
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 500.0f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

void AHWCharacter::BeginPlay() {
    Super::BeginPlay();
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AHWCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!Input) {
        return;
    }

    Input->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
    Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
    Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHWCharacter::Move);
    Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHWCharacter::Look);
    Input->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AHWCharacter::Look);
    Input->BindAction(SprintAction, ETriggerEvent::Started, this, &AHWCharacter::StartSprint);
    Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &AHWCharacter::StopSprint);
    Input->BindAction(SprintAction, ETriggerEvent::Canceled, this, &AHWCharacter::StopSprint);
    Input->BindAction(InteractAction, ETriggerEvent::Started, this, &AHWCharacter::Interact);
}

void AHWCharacter::Move(const FInputActionValue& Value) {
    const FVector2D Input = Value.Get<FVector2D>();
    DoMove(Input.X, Input.Y);
}

void AHWCharacter::DoMove(float Right, float Forward) {
    if (!Controller) return;
    const FRotator Yaw(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X), Forward);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Right);
}

void AHWCharacter::Look(const FInputActionValue& Value) {
    const FVector2D Input = Value.Get<FVector2D>();
    DoLook(Input.X, Input.Y);
}

void AHWCharacter::DoLook(float Yaw, float Pitch) {
    AddControllerYawInput(Yaw);
    AddControllerPitchInput(Pitch);
}

void AHWCharacter::StartSprint() {
    if (Stamina <= 0.0f) return;
    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AHWCharacter::StopSprint() {
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AHWCharacter::Interact() {
    OnInteraction.Broadcast();
}

void AHWCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    if (bIsSprinting) {
        Stamina = FMath::Max(0.0f, Stamina - StaminaDrain * DeltaTime);
        if (Stamina <= 0.0f) StopSprint();
    } else {
        Stamina = FMath::Min(100.0f, Stamina + StaminaRecovery * DeltaTime);
    }

    if (GEngine && IsLocallyControlled()) {
        GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green,
            FString::Printf(TEXT("Stamina: %.0f / 100"), Stamina));
    }
}

void AHWCharacter::DoJumpStart() {
    Jump();
}
void AHWCharacter::DoJumpEnd() {
    StopJumping();
}
