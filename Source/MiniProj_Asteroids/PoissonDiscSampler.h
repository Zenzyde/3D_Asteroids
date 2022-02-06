// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MINIPROJ_ASTEROIDS_API PoissonDiscSampler
{
public:
	static TArray<FVector> SamplePoissonDiscPositions(FVector sampleRegion, float cellRadius, int numInsertionAttempts = 20);

private:
	static bool IsValid(FVector candidate, FVector sampleRegionSize, float cellSize, float radius, TArray<FVector> points, TArray<int> grid, int gridWidth, int gridHeight);
};
