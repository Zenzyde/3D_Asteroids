// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Proj_Asteroids3DV2 : ModuleRules
{
	public Proj_Asteroids3DV2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "NavigationSystem", "UMG" });
	}
}
