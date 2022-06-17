// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Fletched : ModuleRules
{
	public Fletched(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "SlateCore" });
	}
}
