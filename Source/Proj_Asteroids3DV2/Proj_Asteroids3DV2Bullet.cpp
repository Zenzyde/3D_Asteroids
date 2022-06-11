// Fill out your copyright notice in the Description page of Project Settings.

#include "Proj_Asteroids3DV2Bullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine.h"

// Sets default values
AProj_Asteroids3DV2Bullet::AProj_Asteroids3DV2Bullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create projectile movement
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	// Create collision sphere
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));

	// Create projectile mesh
	projectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	projectileMesh->SetupAttachment(collisionComp);

	// Set variable defaults
	MoveSpeed = 700.f;
}

// Called when the game starts or when spawned
void AProj_Asteroids3DV2Bullet::BeginPlay()
{
	Super::BeginPlay();

	// Initialize bullet movement simply by setting velocity once in the forward direction with magnitude of MoveSpeed var
	movementComp->Velocity = GetActorForwardVector() * MoveSpeed;

	// Bind collision event
	collisionComp->OnComponentHit.AddDynamic(this, &AProj_Asteroids3DV2Bullet::OnHit);
}

// Called every frame
void AProj_Asteroids3DV2Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called on collision
void AProj_Asteroids3DV2Bullet::OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComponent, FVector NormalImpulse, const FHitResult &hit)
{
	// FString debugMessage = FString::Printf(TEXT("Hit %s"), *OtherActor->GetName());
	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, debugMessage);
}
