// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Broadcast_TestMap : ModuleRules
{
	public Broadcast_TestMap(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
