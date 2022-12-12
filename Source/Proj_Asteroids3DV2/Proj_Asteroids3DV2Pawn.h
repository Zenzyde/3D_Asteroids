// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Proj_Asteroids3DV2Pawn.generated.h"

UCLASS(Config = Game)
class AProj_Asteroids3DV2Pawn : public APawn
{
	GENERATED_BODY()

	// Health component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	class UCharacterStateComponent *HealthComponent;

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *PlaneMesh;

	/** StaticMesh component that will be the visuals for our flying pawn shield */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *PlaneShieldMesh;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent *SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent *Camera;

	// Perception Stimuli Source component
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI Sense", meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionStimuliSourceComponent *PerceptionStimuliSourceComponent;

	// Objective direction arrow mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent *ObjectiveDirectionMesh;

public:
	AProj_Asteroids3DV2Pawn();

	// Begin AActor overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent *MyComp, class AActor *Other, class UPrimitiveComponent *OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult &Hit) override;
	// End AActor overrides

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &HitResult);

	// Called when damage has been registered
	virtual float TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCause) override;

	// Getter for Maxspeed
	UFUNCTION(BlueprintPure)
	float GetMaxSpeed();

	// Getter for current forward speed
	UFUNCTION(BlueprintPure)
	float GetCurrentForwardSpeed();

	// Call to set the spaceships ability to turn
	UFUNCTION(BlueprintCallable)
	void SetTurnState(bool state);

	FORCEINLINE UCharacterStateComponent *GetHealthState() const { return HealthComponent; }

protected:
	// Begin APawn overrides
	virtual void SetupPlayerInputComponent(class UInputComponent *InputComponent) override; // Allows binding actions/axes to functions
	// End APawn overrides

	/** Bound to the thrust action */
	void ActivateThrust();
	void DeactivateThrust();
	void HandleThrustInput(float DeltaSeconds);

	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

	void RotateUpInput(float Val);

	void RotateRightInput(float Val);

	// Called when firing projectile
	void SpawnProjectile();

	// Called when exiting game
	void ExitGame();

	UFUNCTION(BlueprintCallable)
	void DelayedInitialization();

public:
	// Bullet actor to fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AProj_Asteroids3DV2Bullet> Bullet;

	// Bullet spawning position offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ProjectileSpawnOffset;

	// Fire rate -- how fast does the spaceship fire projectiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate;

	// Current FireRate time -- for use in widget
	UPROPERTY(BlueprintReadOnly)
	float CurrentFireRateTime;

	// Max thrust amount -- for how long can we boost
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxThrustAmount;

	// Current thrust amount -- for use in widget
	UPROPERTY(BlueprintReadOnly)
	float CurrentThrustAmount;

	// Thrust depletion rate -- how fast does thrust amount deplete per second
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThrustDepletionRate;

	// Thrust accumulation rate -- how fast does thrust amount accumulate per second
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThrustAccumulationRate;

	// Spawn invulnerability countdown time -- how long spaceship should be invulnerable at spawn to avoid being killed by asteroids
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InvulnerabilitySpawnTimer;

private:
	/** How quickly forward speed changes */
	UPROPERTY(Category = Plane, EditAnywhere)
	float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category = Plane, EditAnywhere)
	float TurnSpeed;

	UPROPERTY(Category = Plane, EditAnywhere)
	float MoveSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Pitch, EditAnywhere)
	float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category = Yaw, EditAnywhere)
	float MinSpeed;

	/** Current forward speed */
	float CurrentForwardSpeed;

	float CurrentUpSpeed;

	float CurrentRightSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;

	// Flag that marks whether thrust currently being used
	bool bIsThrusting;

	// Flag that marks whether the ship can turn or not
	bool bCanTurn;

	// Actor pointer to portal once it's spawned
	AActor *Portal;

public:
	/** Returns PlaneMesh subobject **/
	FORCEINLINE class UStaticMeshComponent *GetPlaneMesh() const { return PlaneMesh; }
	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent *GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent *GetCamera() const { return Camera; }
};
