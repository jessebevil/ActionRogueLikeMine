// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	HealthMax = 100;
	Health = HealthMax;
}

bool UAttributeComponent::IsAlive() const {
	return Health > 0.0f;
}

bool UAttributeComponent::ApplyHealthChange(float Delta) {
	float oldHealth = Health;//Save original health for later.

	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = Health - oldHealth;//Compare old health with new health to see if there was a change.

	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);

	return ActualDelta != 0;//maybe check if the delta did anything? Did they live?
}

bool UAttributeComponent::IsFullHealth() const {
	return Health == HealthMax;
}

float UAttributeComponent::GetHealthMax() const {
	return HealthMax;
}
