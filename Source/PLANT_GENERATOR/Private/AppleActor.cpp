#include "AppleActor.h"
#include "Util.h"

AAppleActor::AAppleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Apple = this->GetStaticMeshComponent();
	Apple->SetMobility(EComponentMobility::Movable);

	
	m_MinSaturationVariation = 0.8f;
	m_MaxSaturationVariation = 1.2f;
	m_MinBrightnessVariation = 4.0f;
	m_MaxBrightnessVariation = 5.f;
	m_MinContrastVariation = 0.8f;
	m_MaxContrastVariation = 1.2f;
	m_minScale = 0.66;
	m_maxScale = 1.5;
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
	const bool IsRed = (bool)parameters.FindRef("IsRed", false);
	UStaticMesh* RandomAppleMesh = nullptr;

	if (IsRed)
		RandomAppleMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Apples/Apple2"));
	else
		RandomAppleMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Apples/Apple1"));
		
	Apple->SetStaticMesh(RandomAppleMesh);

	//Apple->SetWorldScale3D(FVector(10, 10, 10));

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

	float randomScale = FMath::FRandRange(m_minScale, m_maxScale);

	FVector NewScale = FVector(
randomScale,
randomScale,
randomScale
);
	
	
	Apple->SetWorldScale3D(NewScale);
	
	FRotator CurrentRotation = Apple->GetComponentRotation();
	float RandomYaw = FMath::FRandRange(0.0f, 360.0f);
	FRotator NewRotation = FRotator(CurrentRotation.Pitch, RandomYaw, CurrentRotation.Roll);

	Apple->SetWorldRotation(NewRotation);
	
}

