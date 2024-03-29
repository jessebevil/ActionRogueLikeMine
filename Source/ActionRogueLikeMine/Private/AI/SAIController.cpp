// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASAIController::BeginPlay() {
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("Behavior tree is nullptr! Please assign BehaviorTree in your AI Controller."))) {
		RunBehaviorTree(BehaviorTree);
	}
}
