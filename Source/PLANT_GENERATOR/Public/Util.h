// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PLANT_GENERATOR_API Util
{
public:
	Util();
	~Util();
	static UStaticMesh* GetRandomMeshFromFolder(const FString& FolderPath);
	static UMaterialInterface* LoadMaterialByName(const FString& MaterialPath);
	/*static UMaterialInstanceConstant* CreateMaterialInstance(UMaterialInterface* Material, FString& PackagePath,
	                                                  FString& BaseAssetName);
	static void SavePackage(UMaterialInstanceConstant* DynMaterial);
	static UMaterialInstanceConstant* CopyMaterialInstanceConstant(UMaterialInstanceConstant* SourceMIC,
	                                                        const FString& BaseAssetName, const FString& PackagePath);*/

	static float GetGoldenAngle() { return 137.5f; } // Phyllotaxis constant
};
