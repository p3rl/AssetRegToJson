// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetRegToJson.h"

#include "RequiredProgramMainCPPInclude.h"

#include "HAL/FileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Serialization/ArrayReader.h"
#include "Serialization/Archive.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "AssetRegistryState.h"

DEFINE_LOG_CATEGORY_STATIC(LogAssetRegToJson, Log, All);

IMPLEMENT_APPLICATION(AssetRegToJson, "AssetRegToJson");

struct FAssetRegToJsonArgs
{
	FString InputPath;
	FString OutputPath;
	bool bIsValid = true;

	static FAssetRegToJsonArgs Parse(const TCHAR* Cmdline)
	{
		FAssetRegToJsonArgs Args;

		Args.bIsValid = FParse::Value(Cmdline, TEXT("Input="), Args.InputPath);

		if (!FParse::Value(Cmdline, TEXT("Output="), Args.OutputPath))
		{
			Args.OutputPath = FPaths::ChangeExtension(Args.InputPath, TEXT("json"));
		}

		return Args;
	}
};

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);

	FAssetRegToJsonArgs Args = FAssetRegToJsonArgs::Parse(FCommandLine::Get());
	if (!Args.bIsValid)
	{
		UE_LOG(LogAssetRegToJson, Display, TEXT("Usage: AssetRegJson.exe -Input=<Path/To/AssetRegistry.bin> -Output=<Path/To/AssetRegistry.json>"));
		FEngineLoop::AppExit();
		return -1;
	}

	FArrayReader SerializedAssetData;
	if (!FFileHelper::LoadFileToArray(SerializedAssetData, *Args.InputPath))
	{
		UE_LOG(LogAssetRegToJson, Display, TEXT("Failed to read asset registry file '%s'"), *Args.InputPath);
		FEngineLoop::AppExit();
		return -1;
	}

	FAssetRegistrySerializationOptions Options(UE::AssetRegistry::ESerializationTarget::ForDevelopment);
	Options.bSerializePackageData = true;

	FAssetRegistryState AssetRegistry;
	if (!AssetRegistry.Serialize(SerializedAssetData, Options))
	{
		UE_LOG(LogAssetRegToJson, Display, TEXT("Failed to load asset registry '%s'"), *Args.InputPath);
		FEngineLoop::AppExit();
		return -1;
	}
	
	TSet<FName> PackageNames;
	TSet<FName> PackageNamesToSkip;
	AssetRegistry.EnumerateAllAssets(PackageNamesToSkip, [&](const FAssetData& AssetData)
	{
		PackageNames.Add(AssetData.PackageName);
		return true;
	});
	
	FString Json;
	TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Json);

	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("package_count"), PackageNames.Num());
	
	Writer->WriteArrayStart(TEXT("packages"));
	{
		for (const FName& PackageName : PackageNames)
		{
			Writer->WriteObjectStart();
			Writer->WriteValue(TEXT("package_name"), PackageName.ToString());

			if (const FAssetPackageData* AssetPackageData = AssetRegistry.GetAssetPackageData(PackageName))
			{
				PRAGMA_DISABLE_DEPRECATION_WARNINGS
				Writer->WriteValue(TEXT("guid"), AssetPackageData->PackageGuid.ToString());
				PRAGMA_ENABLE_DEPRECATION_WARNINGS
				Writer->WriteValue(TEXT("disksize"), AssetPackageData->DiskSize);
				Writer->WriteValue(TEXT("cooked_hash"), LexToString(AssetPackageData->CookedHash));
				Writer->WriteValue(TEXT("file_version_ue"), AssetPackageData->FileVersionUE);
				Writer->WriteValue(TEXT("file_version_licensee_ue"), AssetPackageData->FileVersionLicenseeUE);
			}
			
			Writer->WriteArrayStart(TEXT("assets"));
			TArrayView<FAssetData const* const> Assets = AssetRegistry.GetAssetsByPackageName(PackageName);
			for (const FAssetData* Asset : Assets)
			{
				Writer->WriteObjectStart();
				Writer->WriteValue(TEXT("asset_name"), Asset->AssetName.ToString());
				Writer->WriteValue(TEXT("asset_class"), Asset->AssetClass.ToString());
				Writer->WriteValue(TEXT("package_flags"), int64(Asset->PackageFlags));
				Writer->WriteArrayStart(TEXT("chunk_ids"));
				for (int32 ChunkId : Asset->ChunkIDs)
				{
					Writer->WriteValue(ChunkId);
				}
				Writer->WriteArrayEnd();
				Writer->WriteObjectEnd();
			}
			Writer->WriteArrayEnd();
			
			Writer->WriteObjectEnd();
		}
	}
	Writer->WriteArrayEnd();

	Writer->WriteObjectEnd();
	Writer->Close();

	if (!FFileHelper::SaveStringToFile(Json, *Args.OutputPath))
	{
		UE_LOG(LogAssetRegToJson, Display, TEXT("Failed to save asset registry '%s'"), *Args.OutputPath);
	}
	
	UE_LOG(LogAssetRegToJson, Display, TEXT("Asset registry JSON file saved to '%s'"), *Args.OutputPath);
	
	FEngineLoop::AppExit();
	return 0;
}
