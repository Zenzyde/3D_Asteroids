// Copyright Epic Games, Inc. All Rights Reserved.

#include "Proj_Asteroids3DV2GameMode.h"
#include "Proj_Asteroids3DV2Pawn.h"

AProj_Asteroids3DV2GameMode::AProj_Asteroids3DV2GameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AProj_Asteroids3DV2Pawn::StaticClass();
}
