// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class MINIPROJ_ASTEROIDS_API AProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileActor();

	// Meant for collision detection primarily
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent *projectileMesh;

	// Meant for visuals primarily
	UPROPERTY(VisibleDefaultsOnly)
	class UParticleSystemComponent *projectileVfx;

	UPROPERTY(EditAnywhere)
	float moveSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyHit(class UPrimitiveComponent *MyComp, class AActor *Other, class UPrimitiveComponent *OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult &Hit) override;
};
