#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChoosePoint.generated.h"

UCLASS()
class MECHANICSHW1_API UBTTask_ChoosePoint : public UBTTaskNode {
    GENERATED_BODY()

public:
    UBTTask_ChoosePoint();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    UPROPERTY(EditAnywhere, Category="Search")
    bool bSearchNearNoise = false;

    UPROPERTY(EditAnywhere, Category="Search", meta=(ClampMin="100"))
    float SearchRadius = 300.0f;
};
