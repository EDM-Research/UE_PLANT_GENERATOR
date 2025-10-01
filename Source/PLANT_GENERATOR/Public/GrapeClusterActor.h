// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GrapeClusterActor.generated.h"

UCLASS()
class PLANT_GENERATOR_API AGrapeClusterActor : public AActor
{
    GENERATED_BODY()

public:    
    AGrapeClusterActor();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grape Cluster")
    UStaticMeshComponent* PhysicsRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grape Cluster")
    USplineComponent* RachisSpline;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grape Cluster")
    TArray<USplineMeshComponent*> RachisSplineMeshes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grape Cluster")
    TArray<USplineComponent*> PedicelSplines;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grape Cluster")
    TArray<USplineMeshComponent*> PedicelMeshes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grape Cluster")
    TArray<UStaticMeshComponent*> GrapeMeshes;

    UFUNCTION(BlueprintCallable, Category = "Grape Cluster Generation") 
    void GenerateGrapeCluster(TMap<FString, float> parameters);
    
    UFUNCTION(BlueprintCallable, Category = "Grape Cluster Generation")
    void ClearGrapeCluster();
};