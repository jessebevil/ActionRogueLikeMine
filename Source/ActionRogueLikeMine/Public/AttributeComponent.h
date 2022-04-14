// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, UAttributeComponent*, OwningComp, float, NewHealth, float, Delta);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKEMINE_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttributeComponent();

protected:

	//EditAnywhere - edit in BP editor and per-instance in level.
	//VisibleAnywhere - 'read-only' in editor and level. (Use for Components)
	//EditDefaultsOnly - hide variable per-instance, edit in BP editor only
	//VisibleDefaultsOnly - 'read only' access for variable, only in BP editor (uncommon)
	//EditInstanceOnly - allow only editing of instance (eg. when placed in level)

	//BlueprintReadOnly - read-only in the Blueprint scripting (does not affect 'details' - panel)
	//BlueprintReadWrite - read-write access in Blueprints

	//Category = "Something" - display only for detail panels and blueprint context menu.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes");
	float Health;

	//HealthMax, Stamina, Strength, Mana, Crit chance, etc.

public:
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	bool ApplyHealthChange(float Delta);

};