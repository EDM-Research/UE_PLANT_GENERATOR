#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "AppleActor.generated.h"

UCLASS()
class PLANT_GENERATOR_API AAppleActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	AAppleActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Apple")
	UStaticMeshComponent* Apple;

	UFUNCTION(BlueprintCallable, Category = "Apple Generation") 
	void GenerateApple(TMap<FString, float> parameters);

private:
	float m_MinSaturationVariation;
	float m_MaxSaturationVariation;
	float m_MinBrightnessVariation;
	float m_MaxBrightnessVariation;
	float m_MinContrastVariation;
	float m_MaxContrastVariation;
	float m_minScale;
	float m_maxScale;
};
