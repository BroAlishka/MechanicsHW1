#include "HW2AIController.h"
#include "HW2Enemy.h"
#include "HWCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AHW2AIController::AHW2AIController() {
    PrimaryActorTick.bCanEverTick = true;
    Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
    SetPerceptionComponent(*Perception);
    Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));
    Sight->SightRadius = 900.0f;
    Sight->LoseSightRadius = 1200.0f;
    Sight->PeripheralVisionAngleDegrees = 75.0f;
    Sight->SetMaxAge(2.0f);
    Sight->DetectionByAffiliation.bDetectEnemies = true;
    Sight->DetectionByAffiliation.bDetectFriendlies = true;
    Sight->DetectionByAffiliation.bDetectNeutrals = true;
    Perception->ConfigureSense(*Sight);
    Perception->SetDominantSense(Sight->GetSenseImplementation());

    Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing"));
    Hearing->HearingRange = 1300.0f;
    Hearing->SetMaxAge(3.0f);
    Hearing->DetectionByAffiliation.bDetectEnemies = true;
    Hearing->DetectionByAffiliation.bDetectFriendlies = true;
    Hearing->DetectionByAffiliation.bDetectNeutrals = true;
    Perception->ConfigureSense(*Hearing);
    Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AHW2AIController::OnPerception);

    static ConstructorHelpers::FObjectFinder<UBehaviorTree> Tree(TEXT("/Game/HW2/AI/BT_Guard.BT_Guard"));
    if (Tree.Succeeded()) BehaviorTreeAsset = Tree.Object;
}

void AHW2AIController::OnPossess(APawn* InPawn) {
    Super::OnPossess(InPawn);
    if (!BehaviorTreeAsset || !BehaviorTreeAsset->BlackboardAsset) return;
    UBlackboardComponent* BlackboardComp = nullptr;
    if (!UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp)) return;
    BlackboardComp->SetValueAsVector(TEXT("HomeLocation"), InPawn->GetActorLocation());
    RunBehaviorTree(BehaviorTreeAsset);
}

void AHW2AIController::OnPerception(AActor* Actor, FAIStimulus Stimulus) {
    if (!Cast<AHWCharacter>(Actor) || !GetBlackboardComponent()) return;
    const TSubclassOf<UAISense> Sense = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);
    if (Sense == UAISense_Sight::StaticClass()) {
        if (Stimulus.WasSuccessfullySensed()) {
            GetWorldTimerManager().ClearTimer(NoiseTimer);
            GetBlackboardComponent()->ClearValue(TEXT("HeardLocation"));
            GetBlackboardComponent()->SetValueAsInt(TEXT("SearchStep"), 0);
            GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Actor);
            if (AHW2Enemy* Enemy = Cast<AHW2Enemy>(GetPawn())) {
                Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->ChaseSpeed;
            }
        } else if (GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")) == Actor) {
            StopChasing(Stimulus.StimulusLocation);
        }
    } else if (Sense == UAISense_Hearing::StaticClass() && Stimulus.WasSuccessfullySensed()) {
        if (!GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor"))) {
            GetBlackboardComponent()->SetValueAsVector(TEXT("HeardLocation"), Stimulus.StimulusLocation);
            GetBlackboardComponent()->SetValueAsInt(TEXT("SearchStep"), 0);
            GetWorldTimerManager().SetTimer(NoiseTimer, this, &AHW2AIController::ClearHeardLocation, 15.0f, false);
        }
    }
}

void AHW2AIController::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
    if (!BlackboardComp || !GetPawn()) return;
    AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
    if (Target && FVector::Dist2D(Target->GetActorLocation(), GetPawn()->GetActorLocation()) > 1400.0f) {
        StopChasing(Target->GetActorLocation());
    }
}

void AHW2AIController::StopChasing(const FVector& LastLocation) {
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
    if (!BlackboardComp) return;
    BlackboardComp->ClearValue(TEXT("TargetActor"));
    BlackboardComp->SetValueAsVector(TEXT("HeardLocation"), LastLocation);
    BlackboardComp->SetValueAsInt(TEXT("SearchStep"), 0);
    GetWorldTimerManager().SetTimer(NoiseTimer, this, &AHW2AIController::ClearHeardLocation, 15.0f, false);
    if (AHW2Enemy* Enemy = Cast<AHW2Enemy>(GetPawn())) {
        Enemy->GetCharacterMovement()->MaxWalkSpeed = Enemy->WalkSpeed;
    }
}

void AHW2AIController::ClearHeardLocation() {
    if (GetBlackboardComponent()) {
        GetBlackboardComponent()->ClearValue(TEXT("HeardLocation"));
        GetBlackboardComponent()->SetValueAsInt(TEXT("SearchStep"), 0);
    }
}
