// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms("Win64", "Mac", "Linux")]
[SupportedConfigurations(UnrealTargetConfiguration.Debug, UnrealTargetConfiguration.Development, UnrealTargetConfiguration.Shipping)]
public class AssetRegToJsonTarget : TargetRules
{
	public AssetRegToJsonTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Program;
		LinkType = TargetLinkType.Monolithic;
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
