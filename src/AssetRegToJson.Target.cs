// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms("Win64", "Mac", "Linux")]
[SupportedConfigurations(UnrealTargetConfiguration.Debug, UnrealTargetConfiguration.Development, UnrealTargetConfiguration.Shipping)]
public class AssetRegToJsonTarget : TargetRules
{
	public AssetRegToJsonTarget(TargetInfo Target) : base(Target)
	{
		GlobalDefinitions.Add("IMPLEMENT_ENCRYPTION_KEY_REGISTRATION()=");
		GlobalDefinitions.Add("IMPLEMENT_SIGNING_KEY_REGISTRATION()=");

		Type = TargetType.Program;
		LinkType = TargetLinkType.Monolithic;
		LinkType = TargetLinkType.Modular;
		LaunchModuleName = "AssetRegToJson";

		bBuildDeveloperTools			= false;
		bBuildWithEditorOnlyData		= true;
		bCompileAgainstEngine			= false;
		bCompileAgainstCoreUObject		= false;
		bCompileICU						= false;
		bLegalToDistributeBinary		= true;
		bCompileAgainstApplicationCore	= true;
		bIsBuildingConsoleApplication	= true;
	}
}
