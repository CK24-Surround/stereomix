// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StereoMix : ModuleRules
{
	public StereoMix(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add("StereoMix");

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "ModelViewViewModel", "AnimGraphRuntime", "Niagara"
		});
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"GameplayAbilities", "GameplayTasks", "GameplayTags", "TurboLinkGrpc"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}