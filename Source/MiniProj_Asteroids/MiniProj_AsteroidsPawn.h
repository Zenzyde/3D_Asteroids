// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ProjectileActor.h"
#include "MiniProj_AsteroidsPawn.generated.h"

UCLASS(Config = Game)
class AMiniProj_AsteroidsPawn : public APawn
{
	GENERATED_BODY()

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *PlaneMesh;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent *SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent *Camera;

	UPROPERTY(Category = Custom, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *PlaneRoot;

	UPROPERTY(Category = Custom, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *BoxCollision;

public:
	AMiniProj_AsteroidsPawn();

	// Begin AActor overrides
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent *MyComp, class AActor *Other, class UPrimitiveComponent *OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult &Hit) override;
	// End AActor overrides

	// Explosion on impact with asteroid
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UParticleSystem *explosionVfx;

	/** Returns PlaneMesh subobject **/
	FORCEINLINE class UStaticMeshComponent *GetPlaneMesh() const { return PlaneMesh; }
	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent *GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent *GetCamera() const { return Camera; }

protected:
	// Begin APawn overrides
	virtual void SetupPlayerInputComponent(class UInputComponent *InputComponent) override; // Allows binding actions/axes to functions
	virtual void BeginPlay() override;
	// End APawn overrides

	/** Bound to the thrust axis */
	void ThrustInput(float Val);

	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

	// Update plane mesh rotation
	void UpdatePlaneRotation(float DeltaSeconds);

	// Update firing timer
	void UpdateFireTimer(float DeltaSeconds);

private:
	/** How quickly forward speed changes */
	UPROPERTY(Category = Plane, EditAnywhere)
	float Acceleration;

	/** How quickly pawn can rotate */
	UPROPERTY(Category = Plane, EditAnywhere)
	float RotateSpeed;

	/** How quickly pawn can move */
	UPROPERTY(Category = Plane, EditAnywhere)
	float MoveSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Pitch, EditAnywhere)
	float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category = Yaw, EditAnywhere)
	float MinSpeed;

	/** Max pitch rotation*/
	UPROPERTY(Category = Pitch, EditAnywhere)
	float MaxPitch;

	/** Max yaw rotation */
	UPROPERTY(Category = Yaw, EditAnywhere)
	float MaxYaw;

	/** Current forward speed */
	float CurrentForwardSpeed;

	/** Current up speed */
	float CurrentUpSpeed;

	/** Current right speed */
	float CurrentRightSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;

	bool ReceivingPitchInput;
	bool ReceivingYawInput;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectileActor> projectile;

	void Shoot();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float fireRate;
	float nextFire;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float firingTimer;
};
