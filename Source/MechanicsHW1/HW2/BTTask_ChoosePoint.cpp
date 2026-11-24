#include "BTTask_ChoosePoint.h"
#include "HW2Enemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_ChoosePoint::UBTTask_ChoosePoint() {
    NodeName = TEXT("Choose reachable point");
}

EBTNodeResult::Type UBTTask_ChoosePoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
    AAIController* Controller = OwnerComp.GetAIOwner();
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Controller || !Blackboard || !Controller->GetPawn()) return EBTNodeResult::Failed;

    const FVector Center = Blackboard->GetValueAsVector(bSearchNearNoise ? TEXT("HeardLocation") : TEXT("HomeLocation"));
    const int32 SearchStep = bSearchNearNoise ? Blackboard->GetValueAsInt(TEXT("SearchStep")) : 0;
    if (bSearchNearNoise && SearchStep >= 3) {
        Blackboard->ClearValue(TEXT("HeardLocation"));
        Blackboard->SetValueAsInt(TEXT("SearchStep"), 0);
        return EBTNodeResult::Failed;
    }
    const AHW2Enemy* Enemy = Cast<AHW2Enemy>(Controller->GetPawn());
    const float Radius = bSearchNearNoise ? SearchRadius * (SearchStep + 1) : (Enemy ? Enemy->PatrolRadius : 900.0f);
    UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Controller->GetWorld());
    if (!Nav) return EBTNodeResult::Failed;
    FNavLocation Point;
    bool bFoundPoint = false;
    for (int32 Attempt = 0; Attempt < 5; ++Attempt) {
        if (Nav->GetRandomReachablePointInRadius(Center, Radius, Point)) {
            bFoundPoint = true;
            if (FVector::Dist2D(Controller->GetPawn()->GetActorLocation(), Point.Location) > 150.0f) break;
        }
    }
    if (!bFoundPoint) return EBTNodeResult::Failed;
    Blackboard->SetValueAsVector(TEXT("MoveLocation"), Point.Location);
    if (bSearchNearNoise) Blackboard->SetValueAsInt(TEXT("SearchStep"), SearchStep + 1);
    return EBTNodeResult::Succeeded;
}
