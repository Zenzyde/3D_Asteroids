// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Proj_Asteroids3DV2Bullet.generated.h"

UCLASS()
class PROJ_ASTEROIDS3DV2_API AProj_Asteroids3DV2Bullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProj_Asteroids3DV2Bullet();

	// Mesh component responsible for visuals
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent *projectileMesh;

	// Movement component, borrowing from projectile movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent *movementComp;

	// Collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent *collisionComp;

	// Speed value, how fast does the bullet move
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	// Damage value indicating how much damage is applied to a hit actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when actor is hit by another actor
	UFUNCTION()
	void OnHit(class UPrimitiveComponent *HitComponent, class AActor *OtherActor, UPrimitiveComponent *OtherComponent, FVector NormalImpulse, const FHitResult &hit);
};
