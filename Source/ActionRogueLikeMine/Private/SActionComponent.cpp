// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"
#include "SAction.h"
#include "../ActionRogueLikeMine.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

// Sets default values for this component's properties
USActionComponent::USActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority()) {
		for (TSubclassOf<USAction> ActionClass : DefaultActions) {
			AddAction(GetOwner(), ActionClass);
		}
	}
}


// Called every frame
void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);
	for (USAction* Action : Actions) {
		FColor TextColor = Action->IsRunning() ? FColor::Green : FColor::Red;
		FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s : IsRunning %s : Outer: %s"),
			*GetNameSafe(GetOwner()),
			* Action->ActionName.ToString(),
			Action->IsRunning() ? TEXT("true") : TEXT("false"),
			* GetNameSafe(Action->GetOuter()));

		LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	}
}

void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass) {
	if (!ActionClass) {
		return;
	}

	USAction* NewAction = NewObject<USAction>(GetOwner(), ActionClass);
	if (ensure(NewAction)) {
		NewAction->Initialize(this);

		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator))) {
			NewAction->StartAction(Instigator);
		}
	}
}

void USActionComponent::RemoveAction(USAction* ActionToRemove) {
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
		return;

	Actions.Remove(ActionToRemove);
}

USAction* USActionComponent::GetAction(TSubclassOf<USAction> ActionClass) const
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->IsA(ActionClass))
			return Action;
	}

	return nullptr;
}

bool USActionComponent::StartActionByName(AActor* InstigatorActor, FName ActionName) {
	for (USAction* Action : Actions) {
		if (Action && Action->ActionName == ActionName) {
			if (!Action->CanStart(InstigatorActor)) {
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
				continue;
			}

			//If it's the client. Tell the server you want to start the action. If it's the server, don't repeat it forever.
			if (!GetOwner()->HasAuthority())
				ServerStartAction(InstigatorActor, ActionName);

			Action->StartAction(InstigatorActor);
			return true;
		}
	}

	return false;
}

bool USActionComponent::StopActionByName(AActor* InstigatorActor, FName ActionName) {
	for (USAction* Action : Actions) {
		if (Action && Action->ActionName == ActionName) {
			if (Action->IsRunning()) {
				Action->StopAction(InstigatorActor);
				return true;
			}	
		}
	}

	return false;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName) {
	StartActionByName(Instigator, ActionName);
}


void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}

bool USActionComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) {
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (USAction* Action : Actions) {
		if (Action)
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
	}

	return WroteSomething;
}
