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

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;

	AttackAnimDelay = 0.2f;
	TimeToHitParamName = "TimeToHit";//WTf is this for?
	HandSocketName = "Muzzle_01";

}

void ASCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

//Function that controls the creation of an AActor (Projectile) at a designated location on the character.
void ASCharacter::PrimaryAttack() {
	StartAttackEffects();
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_CastAnimTimeElapsed, AttackAnimDelay);
}

void ASCharacter::PrimaryAttack_CastAnimTimeElapsed() {
	SpawnProjectile(ProjectileClass);
}

void ASCharacter::BlackHoleAttack() {
	StartAttackEffects();
	GetWorldTimerManager().SetTimer(TimerHandle_BlackHoleAttack, this, &ASCharacter::BlackHoleAttack_CastAnimTimeElapsed, AttackAnimDelay);
}

void ASCharacter::BlackHoleAttack_CastAnimTimeElapsed() {
	SpawnProjectile(BlackHoleClass);
}

void ASCharacter::Dash() {
	StartAttackEffects();
	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASCharacter::Dash_CastAnimTimeElapsed, AttackAnimDelay);
}

void ASCharacter::Dash_CastAnimTimeElapsed() {
	SpawnProjectile(DashClass);
}

void ASCharacter::StartAttackEffects() {
	PlayAnimMontage(AttackAnim);
	UGameplayStatics::SpawnEmitterAttached(CastingEffect, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn) {
	if (ensureAlways(ClassToSpawn)) {
		FVector HandLocation = GetMesh()->GetSocketLocation(HandSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;

		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		//Ignore Player
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector TraceStart = CameraComp->GetComponentLocation();

		//endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
		FVector TraceEnd = CameraComp->GetComponentLocation() + (GetControlRotation().Vector() * 2500);

		//find new direction/rotation from hand pointing to impact point in world.
		FRotator ProjRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();

		FHitResult Hit;
		// returns true if we got to a blocking hit
		if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, Shape, Params)) {
			// Overwrite TraceEnd with impact point in world
			TraceEnd = Hit.ImpactPoint;
		}

		FTransform SpawnTM = FTransform(ProjRotation, HandLocation);
		GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTM, SpawnParams);
	}
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
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);

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

void ASCharacter::PrimaryInteract() {
	if (InteractionComp) InteractionComp->PrimaryInteract();
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewHealth, float Delta) {
	if (Delta < 0.0f) {
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
	}

	if (NewHealth <= 0 && Delta < 0.0f) {
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
}

