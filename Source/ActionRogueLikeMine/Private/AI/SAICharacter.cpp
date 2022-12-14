// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "AttributeComponent.h"
#include "BrainComponent.h"
#include "SWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SActionComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
    AttributeComp = CreateDefaultSubobject<UAttributeComponent>("AttributeComp");
    ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

    // Ensures we receive a controlled when spawned in the level by our gamemode
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // Disabled on capsule to let projectiles pass through capsule and hit mesh instead
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
    // Enabled on mesh to react to incoming projectiles
    GetMesh()->SetGenerateOverlapEvents(true);

    TimeToHitParamName = "TimeToHit";
    TargetActorKey = "TargetActor";
}

void ASAICharacter::SetTargetActor(AActor* NewTarget) {
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC) {
        AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);
    }
}

AActor* ASAICharacter::GetTargetActor() const {
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC) {
		return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
	}

	return nullptr;
}

void ASAICharacter::PostInitializeComponents() {
    Super::PostInitializeComponents();

    PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
    AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
} 

void ASAICharacter::OnPawnSeen(APawn* Pawn) {
    // Ignore if target already set
    if (GetTargetActor() != Pawn) {
        SetTargetActor(Pawn);
        USWorldUserWidget* NewWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);
        if (NewWidget) {
            NewWidget->AttachedActor = this;
            // Index of 10 (or anything higher than default of 0) places this on top of any other widget.
            // May end up behind the minion health bar otherwise.
            NewWidget->AddToViewport(10);
        }
    }
    
    //DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewHealth, float Delta) {
    if (Delta < 0.0f) {
        GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
        
        if (InstigatorActor != this) {
            SetTargetActor(InstigatorActor);
        }

        if (ActiveHealthBar == nullptr) {
            ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
            if (ActiveHealthBar) {
                ActiveHealthBar->AttachedActor = this;
                ActiveHealthBar->AddToViewport();
            }
        }

        GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

        if (NewHealth <= 0) {
            //Stop BT
            AAIController* AIC = Cast<AAIController>(GetController());
            if (AIC) {
                AIC->GetBrainComponent()->StopLogic("Killed");
            }

            //Ragdoll
            GetMesh()->SetAllBodiesSimulatePhysics(true);
            GetMesh()->SetCollisionProfileName("Ragdoll");

            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            GetCharacterMovement()->DisableMovement();

            //Set lifespan
            SetLifeSpan(10.0f);
        }
    }
}