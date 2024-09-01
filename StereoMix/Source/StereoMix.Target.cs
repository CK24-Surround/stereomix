// Copyright Epic Games, Inc. All Rights Reserved.

using Microsoft.Extensions.Logging;

using UnrealBuildTool;

public class StereoMixTarget : TargetRules
{
	public static bool HasWarnedAboutShared;

	public StereoMixTarget(TargetInfo target) : base(target)
	{
		Type = TargetType.Client;
		ExtraModuleNames.Add("StereoMix");

		ApplySharedStereoMixTargetSettings(this);
	}

	internal static void ApplySharedStereoMixTargetSettings(TargetRules target)
	{
		var logger = target.Logger;

		target.DefaultBuildSettings = BuildSettingsVersion.V5;
		target.IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;

		var bIsTest = target.Configuration == UnrealTargetConfiguration.Test;
		var bIsShipping = target.Configuration == UnrealTargetConfiguration.Shipping;
		var bIsDedicatedServer = target.Type == TargetType.Server;
		if (target.BuildEnvironment == TargetBuildEnvironment.Unique)
		{
			target.ShadowVariableWarningLevel = WarningLevel.Error;
			if (bIsShipping)
			{
				target.bUseLoggingInShipping = true;

				if (bIsDedicatedServer)
				{
					target.bUseChecksInShipping = true;
					target.bUseExecCommandsInShipping = true;
					target.GlobalDefinitions.Add("UE_ALLOW_MAP_OVERRIDE_IN_SHIPPING=1");
				}
				else
				{
					// Make sure that we validate certificates for HTTPS traffic
					target.bDisableUnverifiedCertificates = true;

					// Uncomment these lines to lock down the command line processing
					// This will only allow the specified command line arguments to be parsed
					//Target.GlobalDefinitions.Add("UE_COMMAND_LINE_USES_ALLOW_LIST=1");
					//Target.GlobalDefinitions.Add("UE_OVERRIDE_COMMAND_LINE_ALLOW_LIST=\"-space -separated -list -of -commands\"");

					// Uncomment this line to filter out sensitive command line arguments that you
					// don't want to go into the log file (e.g., if you were uploading logs)
					//Target.GlobalDefinitions.Add("FILTER_COMMANDLINE_LOGGING=\"-some_connection_id -some_other_arg\"");
				}

				// Disable reading generated/non-ufs ini files
				target.bAllowGeneratedIniWhenCooked = false;
				target.bAllowNonUFSIniWhenCooked = false;
			}

			if (target.Type != TargetType.Editor)
			{
				// We don't use the path tracer at runtime, only for beauty shots, and this DLL is quite large
				target.DisablePlugins.Add("OpenImageDenoise");
			}
		}
		else
		{
			// !!!!!!!!!!!! WARNING !!!!!!!!!!!!!
			// Any changes in here must not affect PCH generation, or the target
			// needs to be set to TargetBuildEnvironment.Unique

			if (target.Type != TargetType.Editor)
			{
				if (HasWarnedAboutShared)
				{
					return;
				}

				HasWarnedAboutShared = true;
				logger.LogWarning("Dynamic target options are disabled when packaging from an installed version of the engine");
			}
		}
	}
}