// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StereoMix : ModuleRules
{
    public StereoMix(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableExceptions = true;
        PublicIncludePaths.Add("StereoMix");

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "AnimGraphRuntime", "Niagara", "GameLiftServerSDK"
        });
        PrivateDependencyModuleNames.AddRange(new[]
        {
            "GameplayAbilities", "GameplayTasks", "GameplayTags", "RenderDoc", "HTTP", "Json", "JsonUtilities"
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}