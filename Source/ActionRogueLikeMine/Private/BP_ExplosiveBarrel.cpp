// Fill out your copyright notice in the Description page of Project Settings.

#include "BP_ExplosiveBarrel.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values
ABP_ExplosiveBarrel::ABP_ExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("SphereComp");
	RootComponent = MeshComp;

	ForceComp = CreateDefaultSubobject<URadialForceComponent>("EffectComp");
	ForceComp->SetupAttachment(MeshComp);

	// Leaving this on applies small constant force via component `tick`
	ForceComp->SetAutoActivate(false);

	ForceComp->Radius = 750;
	ForceComp->ImpulseStrength = 2500.0f;// Alternative: 20000.0 if bImpulseVelChange = false
	//Optional, ignores 'Mass' of other objects (if false, the impulse strength will be much higher to push most objects
	//Depending on mass
	ForceComp->bImpulseVelChange = true;

	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

// Called when the game starts or when spawned
void ABP_ExplosiveBarrel::BeginPlay()
{
	Super::PostInitializeComponents();

	MeshComp->OnComponentHit.AddDynamic(this, &ABP_ExplosiveBarrel::OnActorHit);
	Super::BeginPlay();
}

void ABP_ExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpluse, const FHitResult& Hit) {
	FString OtherActorName = GetNameSafe(OtherActor);
	if (OtherActorName.StartsWith("Player") == false) {
		ForceComp->FireImpulse();

		UE_LOG(LogTemp, Log, TEXT("OnActorHit in Explosive Barrel"));
		UE_LOG(LogTemp, Log, TEXT("OtherActor: %s, at game time: %2.2f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds);

		/*FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
		DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
		FString Output = FString::Printf(TEXT("OtherActor: %s, at game time: %2.2f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds, *Hit.ImpactPoint.ToString());
		DrawDebugString(GetWorld(), Hit.ImpactPoint, Output);*/
	}
}