// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterStateComponent.h"
#include "Proj_Asteroids3DV2GameMode.h"
#include "Containers/Queue.h"
#include "Containers/Map.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine.h"
#include "GameFramework/DamageType.h"

// Sets default values for this component's properties
UCharacterStateComponent::UCharacterStateComponent(const FObjectInitializer &ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Set default immortal state
	bIsImmortal = false;
}

// Called when the game starts
void UCharacterStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set default health
	CurrentHealth = MaxHealth;

	// for (size_t i = 0; i < DamageModifiers.Num(); i++)
	// {
	// 	DamageModifierDict.Add(DamageModifiers[i].EffectType, DamageModifiers[i].EffectModifier);
	// }
}

// Called every frame
void UCharacterStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Damage over time effects
	// if (ActiveStatusEffects.Num() > 0)
	// {
	// 	for (size_t i = ActiveStatusEffects.Num() - 1; i >= 0; i--)
	// 	{
	// 		EDamageEffectType effectType = ActiveStatusEffects[i].EffectType;

	// 		// Apply effect over time and decrease effect timer
	// 		ACharacter *character = Cast<ACharacter>(GetOwner());
	// 		switch (effectType)
	// 		{
	// 		case EDamageEffectType::Stun:
	// 			if (ActiveStatusEffects[i].EffectLifetime <= 0.f)
	// 			{
	// 				character->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	// 				bIsStunned = false;
	// 			}
	// 			break;
	// 		case EDamageEffectType::Poison:
	// 			if (ActiveStatusEffects[i].EffectLifetime <= 0.f)
	// 			{
	// 				bIsPoisoned = false;
	// 			}
	// 			break;
	// 		case EDamageEffectType::Lightning:
	// 			if (ActiveStatusEffects[i].EffectLifetime <= 0.f)
	// 			{
	// 				bIsElectrified = false;
	// 			}
	// 			break;
	// 		}

	// 		if (DamageModifierDict.Contains(effectType))
	// 			ApplyDamage(*DamageModifierDict.Find(effectType) * ActiveStatusEffects[i].EffectDamage * DeltaTime);

	// 		ActiveStatusEffects[i].EffectLifetime -= DeltaTime;
	// 		if (ActiveStatusEffects[i].EffectLifetime <= 0.f)
	// 			ActiveStatusEffects.RemoveAt(i);
	// 	}
	// }
}

void UCharacterStateComponent::ApplyStatusEffect(UDamageType *DamageEffect, float Damage)
{
	// UDamageTypeLightning *lightning = Cast<UDamageTypeLightning>(DamageEffect);
	// UDamageTypePoison *poison = Cast<UDamageTypePoison>(DamageEffect);
	// UDamageTypeStun *stun = Cast<UDamageTypeStun>(DamageEffect);

	// // Apply one-off effect
	// ACharacter *character = Cast<ACharacter>(GetOwner());

	// if (lightning)
	// {
	// 	FStatusEffectTimer lightningTimer;
	// 	lightningTimer.EffectType = EDamageEffectType::Lightning;
	// 	lightningTimer.EffectLifetime = 5.f;
	// 	lightningTimer.EffectDamage = Damage;
	// 	bIsElectrified = true;
	// 	if (DamageModifierDict.Contains(lightningTimer.EffectType))
	// 		ApplyDamage(*DamageModifierDict.Find(lightningTimer.EffectType) * Damage);
	// 	ActiveStatusEffects.Add(lightningTimer);
	// 	return;
	// }
	// else if (poison)
	// {
	// 	FStatusEffectTimer poisonTimer;
	// 	poisonTimer.EffectType = EDamageEffectType::Poison;
	// 	poisonTimer.EffectLifetime = 5.f;
	// 	poisonTimer.EffectDamage = Damage;
	// 	bIsPoisoned = true;
	// 	if (DamageModifierDict.Contains(poisonTimer.EffectType))
	// 		ApplyDamage(*DamageModifierDict.Find(poisonTimer.EffectType) * Damage);
	// 	ActiveStatusEffects.Add(poisonTimer);
	// 	return;
	// }
	// else if (stun)
	// {
	// 	FStatusEffectTimer stunTimer;
	// 	stunTimer.EffectType = EDamageEffectType::Stun;
	// 	stunTimer.EffectLifetime = 5.f;
	// 	stunTimer.EffectDamage = Damage;
	// 	bIsStunned = true;
	// 	if (DamageModifierDict.Contains(stunTimer.EffectType))
	// 		ApplyDamage(*DamageModifierDict.Find(stunTimer.EffectType) * Damage);
	// 	ActiveStatusEffects.Add(stunTimer);
	// 	character->GetCharacterMovement()->MaxWalkSpeed = 0.f;
	// 	return;
	// }
}

bool UCharacterStateComponent::IsDead()
{
	return CurrentHealth <= 0.0 && !bIsImmortal;
}

bool UCharacterStateComponent::IsImmortal()
{
	return bIsImmortal;
}

void UCharacterStateComponent::SetImmortalState(bool state)
{
	bIsImmortal = state;
}

void UCharacterStateComponent::ApplyDamage(float amount)
{
	if (bIsImmortal)
		return;
	CurrentHealth -= amount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	if (FMath::IsNearlyZero(CurrentHealth) && OnDeath.IsBound())
		OnDeath.Broadcast();

	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s was damaged for %f & has %f HP currently"), *GetOwner()->GetName(), amount, CurrentHealth));
}

void UCharacterStateComponent::ApplyHealth(float amount)
{
	if (bIsImmortal)
		return;
	CurrentHealth += amount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
}

void UCharacterStateComponent::UpdateCharacterState(float Damage, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	UDamageType *DamageEffect = Cast<UDamageType>(DamageEvent.DamageTypeClass);

	if (DamageEffect)
		ApplyStatusEffect(DamageEffect, Damage);
	else
		ApplyDamage(Damage);
}

bool UCharacterStateComponent::IsStunned()
{
	return bIsStunned;
}