#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "HW2AIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBehaviorTree;

UCLASS()
class MECHANICSHW1_API AHW2AIController : public AAIController {
    GENERATED_BODY()

public:
    AHW2AIController();
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

    UPROPERTY(VisibleAnywhere, Category="AI")
    TObjectPtr<UAIPerceptionComponent> Perception;

    UPROPERTY()
    TObjectPtr<UAISenseConfig_Sight> Sight;

    UPROPERTY()
    TObjectPtr<UAISenseConfig_Hearing> Hearing;

    UFUNCTION()
    void OnPerception(AActor* Actor, FAIStimulus Stimulus);

private:
    FTimerHandle NoiseTimer;
    void ClearHeardLocation();
    void StopChasing(const FVector& LastLocation);
};
