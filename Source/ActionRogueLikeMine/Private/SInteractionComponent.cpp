// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.DebugDrawInteraction"), false, TEXT("Enable debug lines for interact component."), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	// You can turn these features off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	TraceRadius = 60.0f;
	TraceDistance = 500.0f;
	CollisionChannel = ECC_WorldDynamic;
	// ...
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* MyPawn = Cast<APawn>(GetOwner());

	if (MyPawn->IsLocallyControlled())
		FindBestInteractable();
}

void USInteractionComponent::FindBestInteractable() {

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);

	FocusedActor = nullptr;//Clear this before trying to populate, to allow it to be nullptr when no focused actor.

	FHitResult Hit;
	bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	if (AActor* HitActor = Hit.GetActor()) {
		if (HitActor->Implements<USGameplayInterface>()) {
			FocusedActor = HitActor;
		}
	}

	if (FocusedActor) {
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass)) {
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if (DefaultWidgetInstance) {
			DefaultWidgetInstance->AttachedActor = FocusedActor;
			if (!DefaultWidgetInstance->IsInViewport()) {
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else {
		if (DefaultWidgetInstance) {
			DefaultWidgetInstance->RemoveFromParent();
		}
	}

	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();
	if (bDebugDraw) {
		DrawDebugSphere(GetWorld(), bBlockingHit ? Hit.ImpactPoint : End, TraceRadius, 32, LineColor, false, 0.1f);
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 0.1f, 0, 2.0f);
	}
}

void USInteractionComponent::PrimaryInteract() {
	ServerInteract(FocusedActor);
}

void USInteractionComponent::ServerInteract_Implementation(AActor* InFocus) {
	if (InFocus == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No focused actor to interact with.");
		return;
	}

	APawn* MyPawn = Cast<APawn>(GetOwner());
	ISGameplayInterface::Execute_Interact(InFocus, MyPawn);
}

