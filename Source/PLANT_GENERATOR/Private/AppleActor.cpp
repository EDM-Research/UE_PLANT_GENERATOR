#include "AppleActor.h"
#include "Util.h"

AAppleActor::AAppleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Apple = this->GetStaticMeshComponent();
	Apple->SetMobility(EComponentMobility::Movable);

	
	m_MinSaturationVariation = 0.5f;
	m_MaxSaturationVariation = 1.5f;
	m_MinBrightnessVariation = 0.5f;
	m_MaxBrightnessVariation = 1.5f;
	m_MinContrastVariation = 0.5f;
	m_MaxContrastVariation = 1.5f;

}

void AAppleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAppleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAppleActor::GenerateApple(TMap<FString, float> parameters)
{
	UStaticMesh* RandomAppleMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Apples/"));
	Apple->SetStaticMesh(RandomAppleMesh);

	Apple->SetWorldScale3D(FVector(10, 10, 10));

	// Create the material instance asset
	UMaterialInterface* BaseMaterial = Apple->GetMaterial(0);
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);

	float Saturation = FMath::FRandRange(m_MinSaturationVariation, m_MaxSaturationVariation);
	float Brightness = FMath::FRandRange(m_MinBrightnessVariation, m_MaxBrightnessVariation);
	float Contrast = FMath::FRandRange(m_MinContrastVariation, m_MaxContrastVariation);
	
	DynMaterial->SetScalarParameterValue("Saturation", Saturation);
	DynMaterial->SetScalarParameterValue("Brightness", Brightness);
	DynMaterial->SetScalarParameterValue("Contrast", Contrast);

	Apple->SetMaterial(0, DynMaterial);
	
}

