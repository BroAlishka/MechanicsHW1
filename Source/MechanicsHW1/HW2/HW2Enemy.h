#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HW2Enemy.generated.h"

UCLASS()
class MECHANICSHW1_API AHW2Enemy : public ACharacter {
    GENERATED_BODY()

public:
    AHW2Enemy();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
    float PatrolRadius = 900.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
    float WalkSpeed = 260.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
    float ChaseSpeed = 450.0f;
};
