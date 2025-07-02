// Fill out your copyright notice in the Description page of Project Settings.

#include "Util.h"
#include "AssetRegistry/AssetRegistryModule.h"
//#include "AssetToolsModule.h"
//#include "Factories/MaterialInstanceConstantFactoryNew.h"
//#include "UObject/SavePackage.h"
//#include "Materials/MaterialInstanceConstant.h"

Util::Util()
{
}

Util::~Util()
{
}

UStaticMesh* Util::GetRandomMeshFromFolder(const FString& FolderPath)
{
	// Get the Asset Registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	// Store found assets
	TArray<FAssetData> AssetDataList;

	// Get all assets in the specified folder
	FARFilter Filter;
	Filter.PackagePaths.Add(*FolderPath);  // Example: "/Game/Meshes/Corn"
	Filter.bRecursivePaths = false;  // Set to true if you want to include subfolders
	Filter.ClassPaths.Add(UStaticMesh::StaticClass()->GetClassPathName());

	// Query the Asset Registry
	AssetRegistryModule.Get().GetAssets(Filter, AssetDataList);

	if (AssetDataList.Num() > 0)
	{
		// Pick a random asset from the list
		int32 RandomIndex = FMath::RandRange(0, AssetDataList.Num() - 1);
		FAssetData RandomAsset = AssetDataList[RandomIndex];

		// Load and return the UStaticMesh
		return Cast<UStaticMesh>(RandomAsset.GetAsset());
	}

	// If no meshes were found, log a warning
	UE_LOG(LogTemp, Warning, TEXT("No static meshes found in folder: %s"), *FolderPath);
	return nullptr;
}

UMaterialInterface* Util::LoadMaterialByName(const FString& MaterialPath)
{
	return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *MaterialPath));
}

/*UMaterialInstanceConstant* Util::CreateMaterialInstance(UMaterialInterface* Material, FString& PackagePath, FString& BaseAssetName)

{
	FString AssetName = BaseAssetName;
	FString PackageName = PackagePath + "/" + AssetName;
	int32 Suffix = 1;

	while (FPackageName::DoesPackageExist(PackageName))
	{
		AssetName = FString::Printf(TEXT("%s_%d"), *BaseAssetName, Suffix++);
		PackageName = PackagePath + "/" + AssetName;
	}

	UPackage* Package = CreatePackage(*PackageName);
	
	auto Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
	Factory->InitialParent = Material;

	auto& AssetTools = FAssetToolsModule::GetModule().Get();
	return Cast<UMaterialInstanceConstant>(
		AssetTools.CreateAsset(AssetName, FPackageName::GetLongPackagePath(PackageName), UMaterialInstanceConstant::StaticClass(), Factory)
	);
}

void Util::SavePackage(UMaterialInstanceConstant* DynMaterial)
{
	DynMaterial->PostEditChange();
	DynMaterial->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(DynMaterial);
	FSavePackageArgs SavePackageArgs;
	SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	UPackage::SavePackage(DynMaterial->GetPackage(), DynMaterial, *FPackageName::LongPackageNameToFilename(DynMaterial->GetOutermost()->GetName(), TEXT(".uasset")), SavePackageArgs);
}

UMaterialInstanceConstant* Util::CopyMaterialInstanceConstant(UMaterialInstanceConstant* SourceMIC, const FString& BaseAssetName, const FString& PackagePath)
{
	if (!SourceMIC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Source MaterialInstanceConstant is null."));
		return nullptr;
	}

	FString AssetName = BaseAssetName;
	FString PackageName = PackagePath + "/" + AssetName;
	int32 Suffix = 1;

	while (FPackageName::DoesPackageExist(PackageName))
	{
		AssetName = FString::Printf(TEXT("%s_%d"), *BaseAssetName, Suffix++);
		PackageName = PackagePath + "/" + AssetName;
	}

	UPackage* Package = CreatePackage(*PackageName);
	
	if (!Package)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create package."));
		return nullptr;
	}

	// Create the new MIC
	UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
	Factory->InitialParent = SourceMIC->Parent;

	UMaterialInstanceConstant* NewMIC = Cast<UMaterialInstanceConstant>(
		Factory->FactoryCreateNew(UMaterialInstanceConstant::StaticClass(), Package, FName(*AssetName),
								  RF_Public | RF_Standalone, nullptr, GWarn));

	if (!NewMIC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create new MIC."));
		return nullptr;
	}

	// Copy scalar parameters
	for (const FScalarParameterValue& Param : SourceMIC->ScalarParameterValues)
	{
		NewMIC->SetScalarParameterValueEditorOnly(Param.ParameterInfo.Name, Param.ParameterValue);
	}

	// Copy vector parameters
	for (const FVectorParameterValue& Param : SourceMIC->VectorParameterValues)
	{
		NewMIC->SetVectorParameterValueEditorOnly(Param.ParameterInfo.Name, Param.ParameterValue);
	}

	// Copy texture parameters
	for (const FTextureParameterValue& Param : SourceMIC->TextureParameterValues)
	{
		NewMIC->SetTextureParameterValueEditorOnly(Param.ParameterInfo.Name, Param.ParameterValue);
	}

	// Finalize
	NewMIC->PostEditChange();
	NewMIC->MarkPackageDirty();

	FAssetRegistryModule::AssetCreated(NewMIC);

	return NewMIC;
}*/