#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HWCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteraction);

UCLASS()
class MECHANICSHW1_API AHWCharacter : public ACharacter {
    GENERATED_BODY()

public:
    AHWCharacter();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category="Input")
    void DoMove(float Right, float Forward);
    UFUNCTION(BlueprintCallable, Category="Input")
    void DoLook(float Yaw, float Pitch);
    UFUNCTION(BlueprintCallable, Category="Input")
    void DoJumpStart();
    UFUNCTION(BlueprintCallable, Category="Input")
    void DoJumpEnd();

    UPROPERTY(BlueprintAssignable, Category="Interaction")
    FOnInteraction OnInteraction;

    UPROPERTY(BlueprintReadOnly, Category="Sprint")
    bool bIsSprinting = false;

    UPROPERTY(BlueprintReadOnly, Category="Sprint")
    float Stamina = 100.0f;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    UCameraComponent* FollowCamera;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* JumpAction;
    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* MoveAction;
    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* LookAction;
    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* MouseLookAction;
    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* SprintAction;
    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* InteractAction;

private:
    float NoiseTimer = 0.0f;
    const float WalkSpeed = 500.0f;
    const float SprintSpeed = 900.0f;
    const float StaminaDrain = 20.0f;
    const float StaminaRecovery = 5.0f;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartSprint();
    void StopSprint();
    void Interact();
};
