// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, UAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, UAttributeComponent*, OwningComp, float, NewHealth, float, Delta);

// Alternative: Share the same signature with generic names.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor, UAttributeComponent*, OwningComp, float, NewValue, float, Delta);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKEMINE_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static UAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes");
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes");
	float HealthMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes");
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes");
	float RageMax;

	//HealthMax, Stamina, Strength, Mana, Crit chance, etc.
	
	/*UPROPERTY(ReplicatedUsing="") //Perhaps to make MultiCast_HealthChanged to unreliable later.
	bool bIsAlive;*/

	//UFUNCTION(NetMulticast, Reliable) //@FIXME: mark as unreliable once we moved the 'state' out of SCharacter
	//void MultiCast_HealthChanged(AActor* InstigatorActor, float NewHealth, float Delta);

	UFUNCTION(NetMulticast, Reliable) //@FIXME: mark as unreliable once we moved the 'state' out of SCharacter
	void MultiCast_AttributeChanged(AActor* InstigatorActor, float NewValue, float Delta);

public:
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool Kill(AActor* IntigatorActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsFullHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealthMax() const;

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	//UPROPERTY(BlueprintAssignable, Category = "Attributes")
	//FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnAttributeChanged OnRageChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnAttributeChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable)
	float GetRage() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);
};
