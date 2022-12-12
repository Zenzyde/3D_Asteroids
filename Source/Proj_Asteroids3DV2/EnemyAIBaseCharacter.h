// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "EnemyAIBaseCharacter.generated.h"

UCLASS()
class PROJ_ASTEROIDS3DV2_API AEnemyAIBaseCharacter : public ACharacter
{
	GENERATED_BODY()

	// Health component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	class UCharacterStateComponent *HealthComponent;

	// Behavior tree asset (for initializing the blackboard so that i can update and get the keys in C++)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree *BehaviorTree;

	// Blackboard component
	UPROPERTY()
	class UBlackboardComponent *BlackboardComponent;

	// AI Sensing component
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI Sense", meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent *AIPerceptionComponent;

	// AI Sight Sense config (not making it public since it wasn't selectable as it's own field for whatever reason, but assigning it in constructor made it editable as usual!)
	class UAISenseConfig *AISightSenseConfig;

	// AI Perception Stimuli Source component
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI Sense", meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionStimuliSourceComponent *AIPerceptionStimuliSourceComponent;

	// Target location for checking against when enemy is moving around -- primarily for flying enemy since pathfinding is a no-go when flying :(
	FVector TargetLocation;

	// Enemy detection FOV
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Sense", meta = (AllowPrivateAccess = "true"))
	// float DetectionFOV;

	// Awareness timer -- timer for giving the player the chance to fly around for a bit before the AI is able to shoot em
	float DetectionAwarenessTimer;

	// Flee player value -- value which decides at what health the AI should switch from attack to flee
	float FleeThreshold;

public:
	// Sets default values for this character's properties
	AEnemyAIBaseCharacter();

	// Pointer to player for pathfinding and other stuff
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
	class AProj_Asteroids3DV2Pawn *Player;

	// AI controller
	UPROPERTY()
	class AEnemyAIBaseController *AIController;

	FORCEINLINE UCharacterStateComponent *GetHealthState() const { return HealthComponent; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when enemy has specifically taken damage
	virtual float TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCause) override;

	// Called when enemy is killed
	virtual void OnDeath();

	// Called on AI sense updates for specific Actor
	UFUNCTION()
	void OnAISenseTargetUpdated(class AActor *SensedActor, FAIStimulus AIStimulus);

	// On possession by controller
	UFUNCTION(BlueprintCallable)
	void InitializationAfterPossession();

	// Called on tick to verify any sensed actor is within FOV
	void VerifyAISensedActors();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Move
	UFUNCTION(BlueprintCallable)
	bool TryMoveToRandomAirLocation(float Radius, float SpheretraceRadius, float ArriveDistance, float FlySpeed, float LookForCollisionDistance);
	// Attack
	UFUNCTION(BlueprintCallable)
	void AttackPlayer(class UClass *Projectile, float Speed, float SpawnOffset);
	// Evade while attacking
	UFUNCTION(BlueprintCallable)
	bool EvadePlayer(float Radius, float SpheretraceRadius, float ArriveDistance, float FlySpeed, float LookForCollisionDistance);
	// Try to fly away from player if too low health
	UFUNCTION(BlueprintCallable)
	bool EscapePlayer(float Radius, float SpheretraceRadius, float ArriveDistance, float FlySpeed, float LookForCollisionDistance);
	//	Fire projectile
	UFUNCTION(BlueprintCallable)
	bool SpawnProjectile(class UClass *Projectile, FVector Position, FVector Direction, float Speed);
};
