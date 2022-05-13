// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"
#include "AttributeComponent.h"

ASPowerup_HealthPotion::ASPowerup_HealthPotion() {

}

void ASPowerup_HealthPotion::Interact_Implementation(APawn* InstigatorPawn) {
	if (!ensure(InstigatorPawn)) {
		return;
	}

	//Logic to handle Coin spending, allowing for a cost associated with health potions.
	//Still need logic to keep, track, spend coins in the first place. So TODO:

	UAttributeComponent* AttributeComp = UAttributeComponent::GetAttributes(InstigatorPawn);
	// Check if not already at max health
	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth()) {
		// Only activate if healed successfully
		if (AttributeComp->ApplyHealthChange(this, AttributeComp->GetHealthMax())) {
			HideAndCooldownPowerup();
		}
	}
}