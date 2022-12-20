// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"
#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"

//Console Commands - Cheats
static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global damage modifier for attribute component."), ECVF_Cheat);

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	HealthMax = 100;
	Health = HealthMax;

	RageMax = 100;
	Rage = 0;

	SetIsReplicatedByDefault(true);
}

bool UAttributeComponent::Kill(AActor* InstigatorActor) {
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}

bool UAttributeComponent::IsActorAlive(AActor* Actor) {
	UAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp) {
		return AttributeComp->IsAlive();
	}

	return false;//If they have no concept of an Attribute component, should they be alive or dead? Dead in this case.
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

float UAttributeComponent::GetRage() const {
	return Rage;
}

bool UAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta) {
	float OldRage = Rage;

	Rage = FMath::Clamp(Rage + Delta, 0.0f, RageMax);

	float ActualDelta = Rage - OldRage;
	if (ActualDelta != 0.0f) {
		OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta);
	}

	return ActualDelta != 0.0f;
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

	//OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);
	if (ActualDelta != 0.0f)
		MultiCast_AttributeChanged(InstigatorActor, Health, ActualDelta);

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

void UAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAttributeComponent, Health);
	DOREPLIFETIME(UAttributeComponent, HealthMax);
	DOREPLIFETIME(UAttributeComponent, Rage);
	DOREPLIFETIME(UAttributeComponent, RageMax);

	//In the event you want only specific owner to see it. COND_InitialOnly = Could be if HealthMax is dynamic per spawn, but we only need it once.
	// You would use this instead of DOREPLIFETIME(UAttributeComponent, HealthMax);
	//DOREPLIFETIME_CONDITION(UAttributeComponent, HealthMax, COND_OwnerOnly);
}

void UAttributeComponent::MultiCast_AttributeChanged_Implementation(AActor* InstigatorActor, float NewValue, float Delta) {
	OnRageChanged.Broadcast(InstigatorActor, this, NewValue, Delta);
	OnHealthChanged.Broadcast(InstigatorActor, this, NewValue, Delta);
}
