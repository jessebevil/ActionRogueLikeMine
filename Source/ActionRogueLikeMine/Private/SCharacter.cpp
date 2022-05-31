// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SInteractionComponent.h"
#include "AttributeComponent.h"
#include "GameFramework/Actor.h"
#include "SActionComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<UAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;

	TimeToHitParamName = "TimeToHit";//WTf is this for?

}

void ASCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

FVector ASCharacter::GetPawnViewLocation() const {
	return CameraComp->GetComponentLocation();
}

//Function that controls the creation of an AActor (Projectile) at a designated location on the character.
void ASCharacter::PrimaryAttack() {
	ActionComp->StartActionByName(this, "PrimaryAttack");
}

void ASCharacter::BlackHoleAttack() {
	ActionComp->StartActionByName(this, "Blackhole");
}

void ASCharacter::Dash() {
	ActionComp->StartActionByName(this, "Dash");
}

//// Called every frame
//void ASCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	// -- Rotation Visualization -- //
//	const float DrawScale = 100.0f;
//	const float Thickness = 5.0f;
//
//	FVector LineStart = GetActorLocation();
//	// Offset to the right of pawn
//	LineStart += GetActorRightVector() * 100.0f;
//	// Set line end in direction of the actor's forward
//	FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
//	// Draw Actor's Direction
//	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);
//
//	FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
//	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
//	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
//}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUpDown", this, &ASCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("BlackHoleAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
}

void ASCharacter::HealSelf(float Amount /* = 100 */) {
	AttributeComp->ApplyHealthChange(this, Amount);
}

	// X = Forward/Back (Red)
	// Y = Right/Left (Green)
	// Z = Up/Down (Blue)

//Handle moving Forward or Back in a dark souls style (Elden Ring now?)
void ASCharacter::MoveForward(float value) {
	FRotator controlRot = GetControlRotation();
	controlRot.Pitch = 0;
	controlRot.Roll = 0;
	AddMovementInput(controlRot.Vector(), value);
}

//Handle moving right or left in a dark souls style (Elden Ring now?)
void ASCharacter::MoveRight(float value) {
	FRotator controlRot = GetControlRotation();
	controlRot.Pitch = 0;
	controlRot.Roll = 0;

	FVector RightVector = FRotationMatrix(controlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, value);
}

void ASCharacter::SprintStart() {
	ActionComp->StartActionByName(this, "Sprint");
}

void ASCharacter::SprintStop() {
	ActionComp->StopActionByName(this, "Sprint");
}

void ASCharacter::PrimaryInteract() {
	if (InteractionComp) InteractionComp->PrimaryInteract();
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewHealth, float Delta) {
	if (Delta < 0.0f) {
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
	}

	if (NewHealth <= 0.0f && Delta < 0.0f) {
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
}

