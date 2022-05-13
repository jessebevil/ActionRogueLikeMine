// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Coin.h"
#include "AttributeComponent.h"

ASPowerup_Coin::ASPowerup_Coin() {

}

void ASPowerup_Coin::Interact_Implementation(APawn* InstigatorPawn) {
	if (!ensure(InstigatorPawn)) {
		return;
	}

	//Implement Coin logic for picking up and increasing coin count.

	//UAttributeComponent* AttributeComp = UAttributeComponent::GetAttributes(InstigatorPawn);
	//// Check if not already at max health
	//if (ensure(AttributeComp) && !AttributeComp->IsFullHealth()) {
	//	// Only activate if healed successfully
	//	if (AttributeComp->ApplyHealthChange(this, AttributeComp->GetHealthMax())) {
	//		HideAndCooldownPowerup();
	//	}
	//}
}
