// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Queue.h"
#include "Proj_Asteroids3DV2GameMode.h"
#include "CharacterStateComponent.generated.h"

USTRUCT(BlueprintType)
struct FDamageEffectModifier
{
	GENERATED_BODY()

public:
	// Damage effect
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	// EDamageEffectType EffectType;

	// Damage modifier
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	float EffectModifier;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJ_ASTEROIDS3DV2_API UCharacterStateComponent : public UActorComponent
{
	GENERATED_BODY()

	// Immortality flag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsImmortal;

	// Stunned flag
	UPROPERTY(BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsStunned;

public:
	// Sets default values for this component's properties
	UCharacterStateComponent(const FObjectInitializer &ObjectInitializer);

	// Character Max Health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float MaxHealth;

	// Current Character Health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	float CurrentHealth;

	// Character resistances and weaknesses, defines how much more/less damage a character would receive from a specific damage type
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	TArray<FDamageEffectModifier> DamageModifiers;

	// Character resistance/weakness map
	// TMap<EDamageEffectType, float> DamageModifierDict;

	// Queued status effects
	// TArray<FStatusEffectTimer> ActiveStatusEffects;

	// Property event for blueprint which broadcasts when the physical character "dies"
	UPROPERTY(BlueprintAssignable, Category = "Character State | Death")
	FOnDeathSignature OnDeath;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	// Called to apply new status effect
	UFUNCTION(BlueprintCallable)
	void ApplyStatusEffect(class UDamageType *DamageEffect, float damage);

	// Get if character is dead
	UFUNCTION(BlueprintCallable)
	bool IsDead();

	// Called to set immortality state
	UFUNCTION(BlueprintCallable)
	void SetImmortalState(bool state);

	// Get if character is immortal
	UFUNCTION(BlueprintCallable)
	bool IsImmortal();

	// Get if character is stunned
	UFUNCTION(BlueprintCallable)
	bool IsStunned();

	// Called to apply damage
	UFUNCTION(BlueprintCallable)
	void ApplyDamage(float amount);

	// Called to apply healing
	UFUNCTION(BlueprintCallable)
	void ApplyHealth(float amount);

	// Called by owning pawn/character when receiving the TakeAnyDamage event -- needs the character to call this method when taking damage
	void UpdateCharacterState(float Damage, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser);
};