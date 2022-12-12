// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIBaseController.generated.h"

/**
 *
 */
UCLASS()
class PROJ_ASTEROIDS3DV2_API AEnemyAIBaseController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIBaseController();

	// Overriding UpdateControlRotation because SetFocalPoint literally resitrcts me fomr my desired functionality *upside-down-smile*
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatepawn = true) override;
};
