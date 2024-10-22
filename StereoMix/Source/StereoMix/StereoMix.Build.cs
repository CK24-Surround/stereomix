// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StereoMix : ModuleRules
{
	public StereoMix(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add("StereoMix");

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine", 
			"EngineSettings",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"CommonUI", 
			"AnimGraphRuntime", 
			"Niagara",
			"AdvancedWidgets",
			"Json", 
			"HTTP",
			"StateTreeModule",
			"DeveloperSettings",
			"GameplayMessageRuntime",
			"CADKernel",
			"LevelSequence",
			"MovieScene"
		});

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Slate", 
			"SlateCore",
			"ModelViewViewModel",
			"GameplayAbilities", 
			"GameplayTasks",
			"GameplayTags",
			"TurboLinkGrpc", 
			"ApplicationCore",
			"FMODStudio", 
			"KawaiiPhysics",
			"GameplayMessageRuntime",
			"StructUtils",
			"MediaAssets"
		});
	}
}