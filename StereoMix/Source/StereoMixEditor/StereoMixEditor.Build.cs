using UnrealBuildTool;

public class StereoMixEditor : ModuleRules
{
	public StereoMixEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.Add("StereoMixEditor");
		
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
            "CoreUObject",
            "Engine",
            "EditorFramework",
            "UnrealEd",
            "PhysicsCore",
            "GameplayTagsEditor",
            "GameplayTasksEditor",
            "GameplayAbilities",
            "GameplayAbilitiesEditor",
            "StudioTelemetry",
            "StereoMix"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"InputCore",
            "Slate",
            "SlateCore",
            "ToolMenus",
            "EditorStyle",
            "DataValidation",
            "MessageLog",
            "Projects",
            "DeveloperToolSettings",
            "CollectionManager",
            "SourceControl",
            "Chaos"
		});
	}
}