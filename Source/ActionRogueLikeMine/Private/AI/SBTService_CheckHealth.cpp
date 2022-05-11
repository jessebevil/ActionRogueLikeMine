

#include "AI/SBTService_CheckHealth.h"
#include "AttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"



USBTService_CheckHealth::USBTService_CheckHealth() {
	LowHealthFraction = 0.4f;
}


void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ensure(AIPawn)) {
		UAttributeComponent* AttributeComp = UAttributeComponent::GetAttributes(AIPawn);
		if (ensure(AttributeComp)) {
			bool bLowHealth = (AttributeComp->GetHealth() / AttributeComp->GetHealthMax()) < LowHealthFraction;
			UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
			BlackBoardComp->SetValueAsBool(LowHealthKey.SelectedKeyName, bLowHealth);
		}
	}
}