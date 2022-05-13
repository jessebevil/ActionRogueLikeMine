// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"
#include "SGameModeBase.h"

//Console Commands - Cheats
static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global damage modifier for attribute component."), ECVF_Cheat);

bool UAttributeComponent::IsActorAlive(AActor* Actor) {
	UAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp) {
		return AttributeComp->IsAlive();
	}

	return false;//If they have no concept of an Attribute component, should they be alive or dead? Dead in this case.
}

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	HealthMax = 100;
	Health = HealthMax;
}

bool UAttributeComponent::Kill(AActor* IntigatorActor) {
	return ApplyHealthChange(IntigatorActor, -GetHealthMax());
}

bool UAttributeComponent::IsAlive() const {
	return Health > 0.0f;
}

bool UAttributeComponent::IsFullHealth() const {
	return Health == HealthMax;
}

float UAttributeComponent::GetHealth() {
	return Health;
}

float UAttributeComponent::GetHealthMax() const {
	return HealthMax;
}

bool UAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta) {
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f) {//Console command for God mode. 
		return false;
	}

	if (Delta < 0.0f) {
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= DamageMultiplier;
	}

	float oldHealth = Health;//Save original health for later.

	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = Health - oldHealth;//Compare old health with new health to see if there was a change.

	OnHealthChanged.Broadcast(InstigatorActor, this, Health, Delta);

	if (ActualDelta < 0 && Health == 0.0f) {
		ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (GM) {
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return ActualDelta != 0;//maybe check if the delta did anything? Did they live?
}

UAttributeComponent* UAttributeComponent::GetAttributes(AActor* FromActor) {
	if (FromActor) {
		return Cast<UAttributeComponent>(FromActor->GetComponentByClass(UAttributeComponent::StaticClass()));
	}

	return nullptr;
}
