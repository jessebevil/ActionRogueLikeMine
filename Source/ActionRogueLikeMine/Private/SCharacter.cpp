// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SInteractionComponent.h"

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

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -- Rotation Visualization -- //
	const float DrawScale = 100.0f;
	const float Thickness = 5.0f;

	FVector LineStart = GetActorLocation();
	// Offset to the right of pawn
	LineStart += GetActorRightVector() * 100.0f;
	// Set line end in direction of the actor's forward
	FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	// Draw Actor's Direction
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUpDown", this, &ASCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("BlackHoleAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
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

//Function that controls the creation of an AActor (Projectile) at a designated location on the character.
void ASCharacter::PrimaryAttack() {
	//Always play the animation, should probably make sure you're not already doing the animation.
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.2f);

	//To clear the timer if you need to do so. But isn't needed for this attack.
	//GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
}

void ASCharacter::BlackHoleAttack() {
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::BlackHoleAttack_TimeElapsed, 0.2f);
}

void ASCharacter::BlackHoleAttack_TimeElapsed() {
	//Get the location of the right hand from the skeletal tree.
	//Access by double clicking the skeletal mesh in the inherited mesh component of the PlayerCharacter
	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	//Assign where to spawn the attack using a transform matrix, assign it the hand location.
	FTransform SpawnTM = FTransform(GetControlRotation(), HandLocation);

	//Create SpawnParameters and assign the collision handling method.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	//Tell the world to spawn the projectile, then transform it based on the provided spawn parameters
	GetWorld()->SpawnActor<AActor>(BlackHoleClass, SpawnTM, SpawnParams);
}


void ASCharacter::PrimaryAttack_TimeElapsed() {
	//Get the location of the right hand from the skeletal tree.
	//Access by double clicking the skeletal mesh in the inherited mesh component of the PlayerCharacter
	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	//Assign where to spawn the attack using a transform matrix, assign it the hand location.
	FTransform SpawnTM = FTransform(GetControlRotation(), HandLocation);

	//Create SpawnParameters and assign the collision handling method.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	//Tell the world to spawn the projectile, then transform it based on the provided spawn parameters
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}

void ASCharacter::PrimaryInteract() {
	if (InteractionComp) InteractionComp->PrimaryInteract();
}

