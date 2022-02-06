// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniProj_AsteroidsPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "AsteroidActor.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectileActor.h"
#include "Engine.h"
#include "MiniProj_AsteroidsGameMode.h"

AMiniProj_AsteroidsPawn::AMiniProj_AsteroidsPawn()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create root component meant for steering
	// PlaneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PlaneRoot0"));
	// RootComponent = PlaneRoot;

	// Create box meant for collision-detection
	BoxCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneBoxRoot"));
	// BoxCollision->SetupAttachment(RootComponent);
	RootComponent = BoxCollision;

	// Create static mesh component for visuals
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetupAttachment(RootComponent);
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get()); // Set static mesh

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent); // Attach SpringArm to RootComponent
	SpringArm->TargetArmLength = 160.0f;	   // The camera follows at this distance behind the character
	SpringArm->SocketOffset = FVector(0.f, 0.f, 60.f);
	SpringArm->bEnableCameraLag = false; // Do not allow camera to lag
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera
	Camera->bUsePawnControlRotation = false;							 // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 300.f;
	MoveSpeed = 50.f;
	RotateSpeed = 150.f;
	MaxSpeed = 4000.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 500.f;
	fireRate = 5.f;
}

void AMiniProj_AsteroidsPawn::BeginPlay()
{
	Super::BeginPlay();
	firingTimer = fireRate;
}

void AMiniProj_AsteroidsPawn::Tick(float DeltaSeconds)
{
	const FVector LocalMove = FVector(0.f, CurrentRightSpeed * DeltaSeconds, CurrentUpSpeed * DeltaSeconds);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	UpdatePlaneRotation(DeltaSeconds);

	UpdateFireTimer(DeltaSeconds);

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AMiniProj_AsteroidsPawn::NotifyHit(class UPrimitiveComponent *MyComp, class AActor *Other, class UPrimitiveComponent *OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult &Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	AAsteroidActor *asteroid = Cast<AAsteroidActor>(Other);

	AMiniProj_AsteroidsGameMode *gameMode = Cast<AMiniProj_AsteroidsGameMode>(GetWorld()->GetAuthGameMode());

	if (asteroid && !gameMode->IsPlayerDead)
	{
		gameMode->IsPlayerDead = true;

		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, gameMode, &AMiniProj_AsteroidsGameMode::ReloadGame, 3.f);

		PlaneMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		PlaneMesh->DestroyComponent();
	}

	AddActorLocalOffset(HitNormal * 10, true);
}

void AMiniProj_AsteroidsPawn::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
	// Check if PlayerInputComponent is valid (not NULL)
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("Thrust", this, &AMiniProj_AsteroidsPawn::ThrustInput);
	PlayerInputComponent->BindAxis("MoveUp", this, &AMiniProj_AsteroidsPawn::MoveUpInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMiniProj_AsteroidsPawn::MoveRightInput);

	AMiniProj_AsteroidsGameMode *gameMode = Cast<AMiniProj_AsteroidsGameMode>(GetWorld()->GetAuthGameMode());
	PlayerInputComponent->BindAction("Quit", EInputEvent::IE_Pressed, gameMode, &AMiniProj_AsteroidsGameMode::QuitGame);
	PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &AMiniProj_AsteroidsPawn::Shoot);
}

void AMiniProj_AsteroidsPawn::ThrustInput(float Val)
{
	// Is there any input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void AMiniProj_AsteroidsPawn::MoveUpInput(float Val)
{
	ReceivingPitchInput = FMath::Abs(Val) > 0.0f;

	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * RotateSpeed);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	float TargetUpSpeed = Val * MoveSpeed * Acceleration;

	CurrentUpSpeed = FMath::FInterpTo(CurrentUpSpeed, TargetUpSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	FRotator CurrentRotation = PlaneMesh->GetRelativeRotation();

	FRotator NewRotation = CurrentRotation.Add(TargetPitchSpeed * GetWorld()->GetDeltaSeconds(), 0, 0);

	bool positive = NewRotation.Pitch > 0.0f;

	if (!ReceivingPitchInput && FMath::Abs(CurrentRotation.Pitch) > 0.0f)
	{
		NewRotation = CurrentRotation.Add(positive ? -RotateSpeed * GetWorld()->GetDeltaSeconds() : RotateSpeed * GetWorld()->GetDeltaSeconds(), 0, 0);
		if (FMath::Abs(NewRotation.Pitch) <= 1.5f)
		{
			PlaneMesh->SetRelativeRotation(FRotator(0, CurrentRotation.Yaw, 0));
			return;
		}
	}

	if (FMath::Abs(NewRotation.Pitch) >= MaxPitch)
	{
		PlaneMesh->SetRelativeRotation(FRotator(positive ? MaxPitch : -MaxPitch, CurrentRotation.Yaw, 0));
		return;
	}

	PlaneMesh->SetRelativeRotation(NewRotation);
}

void AMiniProj_AsteroidsPawn::MoveRightInput(float Val)
{
	ReceivingYawInput = FMath::Abs(Val) > 0.0f;

	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * RotateSpeed);

	float TargetRightSpeed = Val * MoveSpeed * Acceleration;

	CurrentRightSpeed = FMath::FInterpTo(CurrentRightSpeed, TargetRightSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	FRotator CurrentRotation = PlaneMesh->GetRelativeRotation();

	FRotator NewRotation = CurrentRotation.Add(0, TargetYawSpeed * GetWorld()->GetDeltaSeconds(), 0);

	bool positive = NewRotation.Yaw > 0.0f;

	if (!ReceivingYawInput && FMath::Abs(CurrentRotation.Yaw) > 0.0f)
	{
		NewRotation = CurrentRotation.Add(0, positive ? -RotateSpeed * GetWorld()->GetDeltaSeconds() : RotateSpeed * GetWorld()->GetDeltaSeconds(), 0);
		if (FMath::Abs(NewRotation.Yaw) <= 1.5f)
		{
			PlaneMesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch, 0, 0));
			return;
		}
	}

	if (FMath::Abs(NewRotation.Yaw) >= MaxYaw)
	{
		PlaneMesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch, positive ? MaxYaw : -MaxYaw, 0));
		return;
	}

	PlaneMesh->SetRelativeRotation(NewRotation);
}

void AMiniProj_AsteroidsPawn::Shoot()
{
	UWorld *world = GetWorld();

	AMiniProj_AsteroidsGameMode *gameMode = Cast<AMiniProj_AsteroidsGameMode>(world->GetAuthGameMode());
	if (gameMode->IsPlayerDead)
		return;

	if (world->GetTimeSeconds() > nextFire)
	{
		world->SpawnActor<AProjectileActor>(projectile, GetActorLocation() + GetActorForwardVector() * 500.f, FRotator(-90, 0, 0));
		nextFire = world->GetTimeSeconds() + fireRate;
		firingTimer = 0.0f;
	}
}

// Legacy, keeping in case for later
void AMiniProj_AsteroidsPawn::UpdatePlaneRotation(float DeltaSeconds)
{
}

void AMiniProj_AsteroidsPawn::UpdateFireTimer(float DeltaSeconds)
{
	UWorld *world = GetWorld();
	AMiniProj_AsteroidsGameMode *gameMode = Cast<AMiniProj_AsteroidsGameMode>(world->GetAuthGameMode());
	if (gameMode->IsPlayerDead)
		return;

	if (firingTimer < fireRate)
	{
		firingTimer += DeltaSeconds;
	}
	else
	{
		firingTimer = fireRate;
	}
}