// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SPowerup_HealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKEMINE_API ASPowerup_HealthPotion : public ASPowerupActor
{
	GENERATED_BODY()

protected:
	UStaticMeshComponent* MeshComp;

	void Interact_Implementation(APawn* InstigatorPawn) override;

	
};
