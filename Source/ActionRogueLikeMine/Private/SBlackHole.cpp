// Fill out your copyright notice in the Description page of Project Settings.


#include "SBlackHole.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles//ParticleSystemComponent.h"
#include "Logging/LogMacros.h"

ASBlackHole::ASBlackHole() {
	MoveComp->InitialSpeed = 400.0f;

	ForceComp = CreateDefaultSubobject<URadialForceComponent>("ForceComp");
	ForceComp->SetupAttachment(RootComponent);
	ForceComp->ImpulseStrength = 5000;
	ForceComp->bIgnoreOwningActor = true;
	ForceComp->ForceStrength = -1000000;
	ForceComp->Radius = 750;
	ForceComp->bImpulseVelChange = true;
	DetonationDelay = 5.0f;
	RageCost = 20.0f;
}

void ASBlackHole::BeginPlay() {
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &ASBlackHole::Explode, DetonationDelay);
}

void ASBlackHole::Explode_Implementation() {
	if (IsPendingKill())
		return;

	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	EffectComp->DeactivateSystem();
	ForceComp->FireImpulse();//Explode everything that it's captured outward.

	MoveComp->StopMovementImmediately();
	SetActorEnableCollision(false);
	Super::Explode_Implementation();
}

void ASBlackHole::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {}