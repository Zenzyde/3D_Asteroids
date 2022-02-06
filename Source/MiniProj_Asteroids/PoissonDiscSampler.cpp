// Fill out your copyright notice in the Description page of Project Settings.

#include "PoissonDiscSampler.h"

TArray<FVector> PoissonDiscSampler::SamplePoissonDiscPositions(FVector sampleRegion, float cellRadius, int numInsertionAttempts)
{
	//Size of a single cell in the grid-area available
	float cellSize = cellRadius / FMath::Sqrt(2);

	int I = FMath::CeilToInt(sampleRegion.X / cellSize);
	int J = FMath::CeilToInt(sampleRegion.Y / cellSize);

	//The available grid-area where positions can be placed.
	//SampleRegionSize / CellSize => amount of rows and columns in the grid
	//Using a 1d instead of 2d array since 1d is easier to work with, just gotta remember to use the formula "width * I + J" when accessing
	//In this case "width" would be I i think..
	TArray<int> grid; // = new int[I * J];
	for (size_t i = 0; i < I * J; i++)
	{
		grid.Add(0);
	}

	//The final list of positions to be returned
	TArray<FVector> points;

	//List of temporary, possible positions to be synced to "points" list later
	TArray<FVector> spawnPoints;

	//First entry, which happens to be the middle of the sample region, don't know how important this is
	spawnPoints.Add(sampleRegion / 2);

	//While there are still temporary, possible positions remaining to check/verify for sync
	while (spawnPoints.Num() > 0)
	{
		//Get random possible position
		int spawnIndex = FMath::RandRange(0, spawnPoints.Num() - 1);
		FVector spawnCenter = spawnPoints[spawnIndex];

		//Prepare check for accepted possible position
		bool candidateAccepted = false;

		//Try for X-amount of attempts to successfully place the possible position
		for (int i = 0; i < numInsertionAttempts; i++)
		{
			//Get a random angle for a direction in polar coords
			float angle = FMath::RandRange(0.0f, 1.0f) * PI * 2;
			//Get a direction from the angle for the position of the possible position
			FVector direction(FMath::Sin(angle), FMath::Cos(angle), 0.0f);
			//Calculate the possible/candidate position from the direction
			FVector candidate(spawnCenter + direction * FMath::RandRange(cellRadius, cellRadius * 2));
			//Check that the candidate/possible position is acceptable
			if (IsValid(candidate, sampleRegion, cellSize, cellRadius, points, grid, I, J))
			{
				//Add the accepted position to the final list
				points.Add(candidate);
				//Add the accepted position to the temporary list
				spawnPoints.Add(candidate);
				//Mark the position in the grid with the length of the final list to distinguish claimed & non-claimed positions
				//(int)Candidate/CellSize => probably converts the candidate position to the grid-format
				grid[I * (int)(candidate.X / cellSize) + (int)(candidate.Y / cellSize)] = points.Num();
				//Set that a candidate has been accepted & break out of the for-loop
				candidateAccepted = true;
				break;
			}
		}

		//If the candidate wasn't accepted, remove it from the temporary list
		if (!candidateAccepted)
		{
			spawnPoints.RemoveAt(spawnIndex);
		}
	}

	return points;
}

bool PoissonDiscSampler::IsValid(FVector candidate, FVector sampleRegionSize, float cellSize, float radius, TArray<FVector> points, TArray<int> grid, int gridWidth, int gridHeight)
{
	//Check that the candidate is within the available region area
	if (candidate.X >= 0 && candidate.X < sampleRegionSize.X && candidate.Y >= 0 && candidate.Y < sampleRegionSize.Y)
	{
		//Get the Cell positions in grid-format probably
		int cellX = (int)(candidate.X / cellSize);
		int cellY = (int)(candidate.Y / cellSize);

		//Calculate start & end search positions around the candidate
		//0, Cell +- 2 & Length - 1 => probably making sure to never go onto the candidate or go outside the available region area
		int searchStartX = FMath::Max(0, cellX - 2);
		int searchEndX = FMath::Min(cellX + 2, gridWidth - 1);
		int searchStartY = FMath::Max(0, cellY - 2);
		int searchEndY = FMath::Min(cellY + 2, gridHeight - 1);

		//Loop through & check the smaller area around the candidate in the given radius
		for (int x = searchStartX; x <= searchEndX; x++)
		{
			for (int y = searchStartY; y <= searchEndY; y++)
			{
				//Get the index of the current position in the grid
				int pointIndex = grid[gridWidth * x + y] - 1;
				//Check if the current position is occupied (i think?)
				if (pointIndex != -1)
				{
					//Calculate the distance from the candidate to the current neighbouring cell
					float squareDistance = FVector::DistSquared(candidate, points[pointIndex]);
					//If current neighbour cell is too close and is occupied, it's invalid
					if (squareDistance < radius * radius)
					{
						return false;
					}
				}
			}
		}
		//No neighbouring cells occupied, candidate position is valid
		return true;
	}
	//Candidate position is outside the available region area somehow, not valid
	return false;
}