// Fill out your copyright notice in the Description page of Project Settings.


#include "SBlackHole.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASBlackHole::ASBlackHole()
{

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	RootComponent = SphereComp;

	ForceComp = CreateDefaultSubobject<URadialForceComponent>("ForceComp");
	ForceComp->SetupAttachment(SphereComp);
	ForceComp->bIgnoreOwningActor;
	ForceComp->ImpulseStrength = 0;
	ForceComp->Radius = 750;
	ForceComp->ForceStrength = -5000000;
	//Optional, ignores 'Mass' of other objects (if false, the impulse strength will be much higher to push most objects
	//Depending on mass
	ForceComp->bImpulseVelChange = true;

	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 400.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	//GetWorldTimerManager().SetTimer(TimerHandle_BlackHoleAttack, this, &ASBlackHole::BlackHoleAttack_TimeElapsed, 5.0f);
}

void ASBlackHole::BlackHoleAttack_TimeElapsed() {
	this->Destroy();
}

void ASBlackHole::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpluse, const FHitResult& Hit) {

	ForceComp->FireImpulse();
}

