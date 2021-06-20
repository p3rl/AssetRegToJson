// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AssetRegToJson : ModuleRules
{
	public AssetRegToJson(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.Add("Runtime/Launch/Public");
		PrivateIncludePaths.Add("Runtime/Launch/Private");		// For LaunchEngineLoop.cpp include
		
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"ApplicationCore",
				"AssetRegistry",
				"Json",
				"Projects",
			}
		);
	}
}
