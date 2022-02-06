// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiniProj_AsteroidsPawn.h"
#include "AsteroidActor.generated.h"

UCLASS()
class MINIPROJ_ASTEROIDS_API AAsteroidActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAsteroidActor();

	// Meant for collision detection primarily
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent *asteroidMesh;

	// Meant for visuals primarily
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UParticleSystemComponent *asteroidVfx;

	// Explosion on impact with bullet or ship
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UParticleSystem *explosionVfx;

	// Cam shake on impact with bullet or ship
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCameraShakeBase> camShake;

	UPROPERTY(EditAnywhere)
	float moveSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyHit(class UPrimitiveComponent *MyComp, class AActor *Other, class UPrimitiveComponent *OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult &Hit) override;
};
