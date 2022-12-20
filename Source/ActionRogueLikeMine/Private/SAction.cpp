// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"
#include "SActionComponent.h"
#include "../ActionRogueLikeMine.h"
#include "Net/UnrealNetwork.h"

void USAction::Initialize(USActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;
}

bool USAction::CanStart_Implementation(AActor* Instigator) {
	if (IsRunning())
		return false;

	USActionComponent* Comp = GetOwningComponent();

	if (Comp->ActiveGameplayTags.HasAny(BlockedTags)) {
		return false;
	}

	return true;
}

void USAction::StartAction_Implementation(AActor* Instigator) {
	//UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	USActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);
	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator) {
	//UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::Red);

	USActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);
	bIsRunning = false;
}

bool USAction::IsRunning() const {
	return bIsRunning;
}

UWorld* USAction::GetWorld() const {
	AActor* Actor = Cast<AActor>(GetOuter());
	if (Actor) {
		return Actor->GetWorld();
	}

	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const {
	return ActionComp;
}

void USAction::OnRep_IsRunning() {
	if (bIsRunning)
		StartAction(nullptr);
	else
		StopAction(nullptr);
}

void USAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, bIsRunning);
	DOREPLIFETIME(USAction, ActionComp);
}
