// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"
#include "Net/UnrealNetwork.h"//Needed for DOREPLIFETIME macro

// Sets default values
ASItemChest::ASItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);

	TargetPitch = 110;

	SetReplicates(true);

}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn) {
	bLidOpened = !bLidOpened;
	OnRep_LidOpened();//triggered automatically for clients. Servers must manually call it.
}

void ASItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(Class, Value) //This is what the C,V means in the macro.
	DOREPLIFETIME(ASItemChest, bLidOpened);
}

void ASItemChest::OnRep_LidOpened() {
	float currPitch = bLidOpened ? TargetPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(currPitch, 0, 0));
}

