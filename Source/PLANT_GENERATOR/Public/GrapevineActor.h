#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"

#include "GrapevineActor.generated.h"

class AGrapeClusterActor;

UCLASS()
class PLANT_GENERATOR_API AGrapevineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrapevineActor();

protected:
	virtual void BeginPlay() override;

public:	
	// Cane/branch -> multiple spawned around a spline
	// Petiole -- link to single leaf to the cane/branch -> multiple spawned on the cane (start is node)
	// --> so multiple nodes and on each you have a petiole or a grape cluster
	// Leaf -- single leaf -> one spawned on the petiole
	// OR grape cluster "GrapeClusterActor.h" -> spawned instead of a petiole at the bottom of a cane (first node)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapevine")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapevine")
	USplineComponent* CordonSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapevine")
	TArray<USplineMeshComponent*> CordonSplineMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapevine")
	TArray<USplineComponent*> CaneSplines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapevine")
	TArray<USplineMeshComponent*> CaneSplineMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapevine")
	TArray<USplineComponent*> PetioleSplines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapevine")
	TArray<bool> bPetioleHasGrapeCluster; // true if petiole has a grape cluster instead of a leaf

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapevine")
	TArray<USplineMeshComponent*> PetioleSplineMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapevine")
	TArray<UStaticMeshComponent*> LeafMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapevine")
	TArray<AGrapeClusterActor*> GrapeClusters;

	UFUNCTION(BlueprintCallable, Category = "Grapevine Generation") 
	void GenerateGrapevine(TMap<FString, float> parameters);

	UFUNCTION(BlueprintCallable, Category = "Grapevine Generation")
	void ClearGrapevine();

private:
	void GenerateCordonSpline(TMap<FString, float> parameters);
	void GenerateCordonSplineMeshes();

	void GenerateCaneSplines(TMap<FString, float> parameters);
	int32 GenerateCaneSplinePoints(int NumCanes, int CaneId, int CaneSegments, int CaneLength,
		const FVector& CaneBendAxis, float CaneBendMagnitude);
	void GenerateAllCaneSplinesMeshes();
	void GenerateCaneSplineMeshes(int32 GlobalCaneId);

	void GenerateAllPetioleSplines(TMap<FString, float> parameters);
	void GenerateCanePetioleSplines(int32 CaneGlobalId, float PetioleLengthMin, float PetioleLengthMax);
	void GenerateAllPetioleSplinesMeshes();
	void GeneratePetioleSplineMeshes(int32 PetioleGlobalId);

	void GenerateAllLeavesAndGrapeClusters(TMap<FString, float> parameters);
};
