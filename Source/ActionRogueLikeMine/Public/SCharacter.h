// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class UAnimMontage;

UCLASS()
class ACTIONROGUELIKEMINE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	//Category is optional
	UPROPERTY(EditAnywhere, Category = "Attack");
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack");
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack");
	TSubclassOf<AActor> BlackHoleClass;

	FTimerHandle TimerHandle_PrimaryAttack;
	FTimerHandle TimerHandle_BlackHoleAttack;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Movement
	void MoveForward(float value);
	void MoveRight(float value);

	//Primary Attack
	void PrimaryAttack();
	void PrimaryAttack_CastAnimTimeElapsed();

	//BlackHoleUltimate
	void BlackHoleAttack();
	void BlackHoleAttack_CastAnimTimeElapsed();

	//Interacting with objects
	void PrimaryInteract();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
