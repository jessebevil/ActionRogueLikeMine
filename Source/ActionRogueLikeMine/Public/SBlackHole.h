// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "SBlackHole.generated.h"

class URadialForceComponent;


UCLASS()
class ACTIONROGUELIKEMINE_API ASBlackHole : public ASProjectileBase
{
	GENERATED_BODY()
public:
	ASBlackHole();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Explode")
	float DetonationDelay;

	UPROPERTY(EditDefaultsOnly, Category = "ForceComp")
	URadialForceComponent* ForceComp;

	FTimerHandle TimerHandle_DelayedDetonate;
	virtual void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void Explode_Implementation() override;
	virtual void BeginPlay() override;
};
