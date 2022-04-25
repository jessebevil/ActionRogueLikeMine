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
	float oldHealth = Health;
	float ActualDelta = Health - oldHealth;

	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);

	return ActualDelta != 0;//maybe check if the delta did anything? Did they live?
}

bool UAttributeComponent::IsFullHealth() const {
	return Health == HealthMax;
}

float UAttributeComponent::GetHealthMax() const {
	return HealthMax;
}
