#include "GrapevineActor.h"
#include "Util.h"
#include "GrapeClusterActor.h"

// Unreal dependencies
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

AGrapevineActor::AGrapevineActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	SceneRoot->SetMobility(EComponentMobility::Movable); 

    CordonSpline = CreateDefaultSubobject<USplineComponent>(TEXT("CordonSpline"));
    CordonSpline->AttachToComponent(SceneRoot, FAttachmentTransformRules::KeepRelativeTransform);
    CordonSpline->SetMobility(EComponentMobility::Movable);
}

void AGrapevineActor::BeginPlay()
{
	Super::BeginPlay();
}

void AGrapevineActor::GenerateGrapevine(TMap<FString, float> parameters)
{
    ClearGrapevine();

    // Generate cordon
    GenerateCordonSpline(parameters);
    GenerateCordonSplineMeshes();

    // Generate canes
    GenerateCaneSplines(parameters);
    GenerateAllCaneSplinesMeshes();

    // Generate petioles
    GenerateAllPetioleSplines(parameters);
    GenerateAllPetioleSplinesMeshes();

	// Generate leaves and grape clusters
	GenerateAllLeavesAndGrapeClusters(parameters);
}

void AGrapevineActor::ClearGrapevine()
{
    for (USplineMeshComponent* Mesh : CordonSplineMeshes)
    {
		if (Mesh && Mesh->IsValidLowLevel())
		{
			Mesh->DestroyComponent();
		}
	}
    CordonSplineMeshes.Empty();
    
    for (USplineComponent* Spline : CaneSplines)
	{
		if (Spline && Spline->IsValidLowLevel())
		{
			Spline->DestroyComponent();
		}
	}
    CaneSplines.Empty();
    
    for (USplineMeshComponent* Mesh : CaneSplineMeshes)
    {
        if (Mesh && Mesh->IsValidLowLevel())
        {
            Mesh->DestroyComponent();
        }
    }
    CaneSplineMeshes.Empty();

    for (USplineComponent* Spline : PetioleSplines)
    {
        if (Spline && Spline->IsValidLowLevel())
        {
            Spline->DestroyComponent();
        }
    }
    PetioleSplines.Empty();
    bPetioleHasGrapeCluster.Empty();

    for (USplineMeshComponent* Mesh : PetioleSplineMeshes)
    {
        if (Mesh && Mesh->IsValidLowLevel())
        {
            Mesh->DestroyComponent();
        }
    }
    PetioleSplineMeshes.Empty();

    for (UStaticMeshComponent* Mesh : LeafMeshes)
    {
        if (Mesh && Mesh->IsValidLowLevel())
        {
            Mesh->DestroyComponent();
        }
    }
    LeafMeshes.Empty();

    for (AGrapeClusterActor* Cluster : GrapeClusters)
	{
		if (Cluster && Cluster->IsValidLowLevel())
		{
			Cluster->ClearGrapeCluster();
			Cluster->Destroy();
		}
	}
    GrapeClusters.Empty();

    CordonSpline->ClearSplinePoints();
}

void AGrapevineActor::GenerateCordonSpline(TMap<FString, float> parameters)
{
    const int CordonSegments = FMath::RandRange(
        parameters.FindRef("CordonSegmentsMin", 3.0f), parameters.FindRef("CordonSegmentsMax", 6.0f)
    );
    const float CordonLength = FMath::RandRange(
        parameters.FindRef("CordonLengthMin", 500.0f), parameters.FindRef("CordonLengthMax", 1100.0f)
    );
    const float CordonBendMagnitude = FMath::RandRange(
        parameters.FindRef("CordonBendMagnitudeMin", 10.0f), parameters.FindRef("CordonBendMagnitudeMax", 300.0f)
    );
    const FVector CordonBendAxis = FVector(
        FMath::RandRange(parameters.FindRef("CordonBendAxisXMin", -0.5f), parameters.FindRef("CordonBendAxisXMax", 0.5f)),
        FMath::RandRange(parameters.FindRef("CordonBendAxisYMin", -0.5f), parameters.FindRef("CordonBendAxisYMax", 0.5f)),
        FMath::RandRange(parameters.FindRef("CordonBendAxisZMin", -0.5f), parameters.FindRef("CordonBendAxisXMax", 0.5f))
    ).GetSafeNormal();

    TArray<FSplinePoint> TempCordonSplinePoints;
    const FVector CordonStart = FVector::ZeroVector;
    const FVector BaseCordonDirection = FVector(0, -1, 0);
	TempCordonSplinePoints.Add(FSplinePoint(0, CordonStart));
    for (int i = 1; i < CordonSegments; i++)
    {
        float Alpha = (float)i / (float)(CordonSegments - 1 + KINDA_SMALL_NUMBER);
        FVector PointLocation = CordonStart + BaseCordonDirection * CordonLength * Alpha;

        FVector BendOffset = CordonBendAxis * CordonBendMagnitude * FMath::RandRange(0.8f, 1.2f);
        PointLocation += BendOffset;

        PointLocation.X += FMath::RandRange(-20.0f, 20.0f);
        PointLocation.Y += FMath::RandRange(-20.0f, 20.0f);

        TempCordonSplinePoints.Add(FSplinePoint(i, PointLocation));
    }
    CordonSpline->ClearSplinePoints();
    CordonSpline->AddPoints(TempCordonSplinePoints);
}

void AGrapevineActor::GenerateCordonSplineMeshes()
{
    for (int i = 0; i < CordonSpline->GetNumberOfSplinePoints() - 1; i++)
    {
        USplineMeshComponent* CordonSplineMesh = NewObject<USplineMeshComponent>(this,
            FName(*FString::Printf(TEXT("CordonSplineMesh_%d"), i)));
        CordonSplineMesh->SetMobility(EComponentMobility::Movable);
        CordonSplineMesh->AttachToComponent(CordonSpline, FAttachmentTransformRules::KeepRelativeTransform);
        CordonSplineMesh->RegisterComponent();

        FVector CordonStart, CordonTangent, CordonEnd, CordonTangentEnd;
        CordonSpline->GetLocationAndTangentAtSplinePoint(i, CordonStart, CordonTangent, ESplineCoordinateSpace::Local);
        CordonSpline->GetLocationAndTangentAtSplinePoint(i + 1, CordonEnd, CordonTangentEnd, ESplineCoordinateSpace::Local);

        UStaticMesh* CordonMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Grape/cylinders/beam"));
        CordonSplineMesh->SetStaticMesh(CordonMesh);
        CordonSplineMesh->SetStartAndEnd(CordonStart, CordonTangent, CordonEnd, CordonTangentEnd);
        CordonSplineMesh->SetForwardAxis(ESplineMeshAxis::X);
        CordonSplineMeshes.Add(CordonSplineMesh);
    }
}

void AGrapevineActor::GenerateCaneSplines(TMap<FString, float> parameters)
{
    const float CaneLengthMin = parameters.FindRef("CaneLengthMin", 50.f);
    const float CaneLengthMax = parameters.FindRef("CaneLengthMax", 500.f);
    const int CaneSegmentsMin = parameters.FindRef("CaneSegmentsMin", 10.0f);
    const int CaneSegmentsMax = parameters.FindRef("CaneSegmentsMax", 20.0f);
    FVector CaneBendAxisMin = FVector(
        parameters.FindRef("CaneBendAxisXMin", 0.0f),
        parameters.FindRef("CaneBendAxisYMin", 0.0f),
        parameters.FindRef("CaneBendAxisZMin", 0.0f)
    );
    FVector CaneBendAxisMax = FVector(
        parameters.FindRef("CaneBendAxisXMax", 0.1f),
        parameters.FindRef("CaneBendAxisYMax", 0.1f),
        parameters.FindRef("CaneBendAxisZMax", 0.1f)
    );
    const float CaneBendMagnitudeMin = parameters.FindRef("CaneBendMagnitudeMin", 2.0f);
    const float CaneBendMagnitudeMax = parameters.FindRef("CaneBendMagnitudeMax", 20.0f);

    int NumCanes = CordonSpline->GetNumberOfSplineSegments() * 4.0f;

    for (int i = 0; i < NumCanes; i++)
    {
        const float CaneLength = FMath::RandRange(CaneLengthMin, CaneLengthMax);
        const int CaneSegments = FMath::RandRange(CaneSegmentsMin, CaneSegmentsMax);
        const FVector CaneBendAxis = FMath::RandPointInBox(FBox(CaneBendAxisMin, CaneBendAxisMax)).GetSafeNormal();
        const float CaneBendMagnitude = FMath::RandRange(CaneBendMagnitudeMin, CaneBendMagnitudeMax);

        GenerateCaneSplinePoints(NumCanes, i, CaneSegments, CaneLength, CaneBendAxis, CaneBendMagnitude);
    }
}

int32 AGrapevineActor::GenerateCaneSplinePoints(int NumCanes, int CaneId, int CaneSegments, int CaneLength,
    const FVector& CaneBendAxis, float CaneBendMagnitude)
{
    // Distribute attachment points along the cordon using its length
    float CordonLengthAtPoint = (float)CaneId / (float)(NumCanes - 1 + KINDA_SMALL_NUMBER)
        * CordonSpline->GetSplineLength();
    FVector AttachmentPointOnCordon = CordonSpline->GetLocationAtDistanceAlongSpline(CordonLengthAtPoint, ESplineCoordinateSpace::Local);

    USplineComponent* CaneSpline = NewObject<USplineComponent>(this, FName(*FString::Printf(TEXT("CaneSpline_%d"), CaneId)));
    CaneSpline->SetMobility(EComponentMobility::Movable);
    CaneSpline->AttachToComponent(CordonSpline, FAttachmentTransformRules::KeepRelativeTransform);
    CaneSpline->RegisterComponent();
    
    TArray<FSplinePoint> TempCaneSplinePoints;
    const FVector CaneStart = AttachmentPointOnCordon;
    const FVector BaseCaneDirection = FVector(FMath::RandRange(-0.1, 0.1), FMath::RandRange(-0.5, 0.5), 1).GetSafeNormal();
	TempCaneSplinePoints.Add(FSplinePoint(0, CaneStart));
    for (int i = 1; i < CaneSegments; i++)
    {
        float Alpha = (float)i / (float)(CaneSegments - 1 + KINDA_SMALL_NUMBER);
        FVector PointLocation = CaneStart + BaseCaneDirection * CaneLength * Alpha;

        FVector BendOffset = CaneBendAxis * CaneBendMagnitude * FMath::RandRange(0.8f, 1.2f);
        PointLocation += BendOffset;

        TempCaneSplinePoints.Add(FSplinePoint(i, PointLocation));
    }
	CaneSpline->ClearSplinePoints();
    CaneSpline->AddPoints(TempCaneSplinePoints);

    return CaneSplines.Add(CaneSpline);
}

void AGrapevineActor::GenerateAllCaneSplinesMeshes()
{
    for (int32 i = 0; i < CaneSplines.Num(); i++)
	{
		GenerateCaneSplineMeshes(i);
	}
}

void AGrapevineActor::GenerateCaneSplineMeshes(int32 GlobalCaneId)
{
    USplineComponent* CaneSpline = CaneSplines[GlobalCaneId];
    for (int i = 0; i < CaneSpline->GetNumberOfSplinePoints() - 1; i++)
    {
        USplineMeshComponent* CaneSplineMesh = NewObject<USplineMeshComponent>(this,
            FName(*FString::Printf(TEXT("CaneSplineMesh_%d_%d"), GlobalCaneId, i)));
        CaneSplineMesh->SetMobility(EComponentMobility::Movable);
        CaneSplineMesh->AttachToComponent(CaneSpline, FAttachmentTransformRules::KeepRelativeTransform);
        CaneSplineMesh->RegisterComponent();

        FVector CaneStart, CaneTangent, CaneEnd, CaneTangentEnd;
        CaneSpline->GetLocationAndTangentAtSplinePoint(i, CaneStart, CaneTangent, ESplineCoordinateSpace::Local);
        CaneSpline->GetLocationAndTangentAtSplinePoint(i + 1, CaneEnd, CaneTangentEnd, ESplineCoordinateSpace::Local);

        UStaticMesh* CaneMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Grape/cylinders/small_beam"));
        CaneSplineMesh->SetStaticMesh(CaneMesh);
        CaneSplineMesh->SetStartAndEnd(CaneStart, CaneTangent, CaneEnd, CaneTangentEnd);
        CaneSplineMesh->SetForwardAxis(ESplineMeshAxis::X);
        CaneSplineMeshes.Add(CaneSplineMesh);
    }
}

void AGrapevineActor::GenerateAllPetioleSplines(TMap<FString, float> parameters)
{
	const float PetioleLengthMin = parameters.FindRef("PetioleLengthMin", 2.0f);
	const float PetioleLengthMax = parameters.FindRef("PetioleLengthMax", 5.0f);

	for (int32 i = 0; i < CaneSplines.Num(); i++)
	{
		GenerateCanePetioleSplines(i, PetioleLengthMin, PetioleLengthMax);
	}
}

void AGrapevineActor::GenerateCanePetioleSplines(int32 CaneGlobalId, float PetioleLengthMin, float PetioleLengthMax)
{
    USplineComponent* CaneSpline = CaneSplines[CaneGlobalId];
    bPetioleHasGrapeCluster.Add(FMath::RandBool()); // Randomly decide if the first petiole has a grape cluster

    TArray<FVector> InitialPetiolePositions;
    TArray<FVector> InitialConnectionPositions;

    int NumPetioles = CaneSpline->GetNumberOfSplinePoints();
    for (int i = 2; i < NumPetioles; i++)
    {
        // Distribute attachment points along the cane using its length
        float CaneLengthAtPoint = FMath::Pow((float)i / (float)(NumPetioles - 1 + KINDA_SMALL_NUMBER), 2.5)
            * CaneSpline->GetSplineLength();

		CaneSpline->GetLocationAtSplineInputKey(i, ESplineCoordinateSpace::Local);

        FVector AttachmentPointOnCane = CaneSpline->GetLocationAtSplineInputKey(i, ESplineCoordinateSpace::Local); //CaneSpline->GetLocationAtDistanceAlongSpline(CaneLengthAtPoint, ESplineCoordinateSpace::Local);
        FVector CaneTangent = CaneSpline->GetTangentAtSplineInputKey(i, ESplineCoordinateSpace::Local);//CaneSpline->GetTangentAtDistanceAlongSpline(CaneLengthAtPoint, ESplineCoordinateSpace::Local);
        CaneTangent.Normalize();

        // Adjust pedicel length based on distance along cane (shorter at bottom)
        float PetioleLengthRatio = 1.0f - (CaneLengthAtPoint / CaneSpline->GetSplineLength());
        float PetioleLength = FMath::RandRange(PetioleLengthMin, PetioleLengthMax) * (0.5f + PetioleLengthRatio * 1.5f);

        // Generate a perpendicular vector to the CaneTangent for radial placement
        FVector OrthoPerpendicular = FVector::CrossProduct(CaneTangent, FVector(0, 0, 1));
        if (OrthoPerpendicular.IsNearlyZero())
        {
            OrthoPerpendicular = FVector::CrossProduct(CaneTangent, FVector(0, 1, 0));
        }
        OrthoPerpendicular.Normalize();

        // Rotate the perpendicular vector around the CaneTangent by golden angle
        FVector RotatedOrtho = OrthoPerpendicular.RotateAngleAxis((float)i * Util::GetGoldenAngle(), CaneTangent);

        // Tilt the petiole direction downwards and slightly outwards
        float DownTiltWeight = FMath::Lerp(0.8f, 0.5f, CaneLengthAtPoint / CaneSpline->GetSplineLength());

        FVector PetioleDir = FMath::Lerp(FVector(0, 0, -1), RotatedOrtho, DownTiltWeight).GetSafeNormal();

        // Add a small cone variation 
        float ConeHalfAngle = 10.0f;
        PetioleDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(PetioleDir, ConeHalfAngle);

        // Calculate initial grape cluster/leaf position
        FVector LeafPos = AttachmentPointOnCane + PetioleDir * PetioleLength;
        InitialPetiolePositions.Add(LeafPos);
        InitialConnectionPositions.Add(AttachmentPointOnCane);

        // Create spline for petiole
        USplineComponent* PetioleSpline = NewObject<USplineComponent>(this,
            FName(*FString::Printf(TEXT("PetioleSpline_%d_%d"), CaneGlobalId, i)));
        PetioleSpline->SetMobility(EComponentMobility::Movable);
        PetioleSpline->AttachToComponent(CaneSpline, FAttachmentTransformRules::KeepRelativeTransform);
        PetioleSpline->RegisterComponent();

        // Petiole spline goes from grape to attachment point on rachis
        PetioleSpline->SetSplinePoints({ AttachmentPointOnCane, LeafPos }, ESplineCoordinateSpace::Local);
        PetioleSplines.Add(PetioleSpline);
    }

    for (int i = 3; i < NumPetioles; i++)
    {
        // All petioles apart from the first one are always leaves
        bPetioleHasGrapeCluster.Add(false);
    }
}

void AGrapevineActor::GenerateAllPetioleSplinesMeshes()
{
    for (int32 i = 0; i < PetioleSplines.Num(); i++)
	{
		GeneratePetioleSplineMeshes(i);
	}
}

void AGrapevineActor::GeneratePetioleSplineMeshes(int32 PetioleGlobalId)
{
    USplineComponent* PetioleSpline = PetioleSplines[PetioleGlobalId];

    USplineMeshComponent* PetioleSplineMesh = NewObject<USplineMeshComponent>(this,
        FName(*FString::Printf(TEXT("PetioleSplineMesh_%d"), PetioleGlobalId)));
    PetioleSplineMesh->SetMobility(EComponentMobility::Movable);
    PetioleSplineMesh->AttachToComponent(PetioleSpline, FAttachmentTransformRules::KeepRelativeTransform);
    PetioleSplineMesh->RegisterComponent();

    FVector PetioleStartPos, PetioleStartTangent, PetioleEndPos, PetioleEndTangent;
    PetioleSpline->GetLocationAndTangentAtSplinePoint(0, PetioleStartPos, PetioleStartTangent, ESplineCoordinateSpace::Local);
    PetioleSpline->GetLocationAndTangentAtSplinePoint(1, PetioleEndPos, PetioleEndTangent, ESplineCoordinateSpace::Local);

    UStaticMesh* PetioleMesh = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Grape/cylinders/small_beam");
    PetioleSplineMesh->SetStaticMesh(PetioleMesh);
    PetioleSplineMesh->SetStartAndEnd(PetioleStartPos, PetioleStartTangent, PetioleEndPos, PetioleEndTangent);
    PetioleSplineMesh->SetForwardAxis(ESplineMeshAxis::X);
    PetioleSplineMeshes.Add(PetioleSplineMesh);
}

void AGrapevineActor::GenerateAllLeavesAndGrapeClusters(TMap<FString, float> parameters)
{
    for (int32 i = 0; i < PetioleSplines.Num(); i++)
	{
		USplineComponent* PetioleSpline = PetioleSplines[i];
        FSplinePoint SplinePoint = PetioleSpline->GetSplinePointAt(
            PetioleSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local);
        // Find rotation so the leaf is spawned following the normal of the surface at the spline point
		
		// Find normal at the spline point
		FVector PetioleTangent = PetioleSpline->GetTangentAtSplinePoint(
			PetioleSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local);
		FVector PetioleNormal = FVector::CrossProduct(PetioleTangent, FVector(0, 0, 1)).GetSafeNormal();
		FQuat PetioleRotation = FQuat::FindBetweenVectors(FVector(0, 0, 1), PetioleNormal);

		if (bPetioleHasGrapeCluster[i])
		{
            // Generate grape cluster
            AGrapeClusterActor* NewGrapeCluster = GWorld->SpawnActor<AGrapeClusterActor>(
                SplinePoint.Position, FRotator::ZeroRotator);
            if (NewGrapeCluster)
            {
                parameters.FindOrAdd("GrapeClusterRachisStartX", SplinePoint.Position.X);
                parameters.FindOrAdd("GrapeClusterRachisStartY", SplinePoint.Position.Y);
                parameters.FindOrAdd("GrapeClusterRachisStartZ", SplinePoint.Position.Z);
				NewGrapeCluster->SetActorScale3D(FVector(0.1f));
                NewGrapeCluster->AttachToComponent(PetioleSpline, FAttachmentTransformRules::KeepRelativeTransform);
                NewGrapeCluster->GenerateGrapeCluster(parameters);
                GrapeClusters.Add(NewGrapeCluster);
            }
		}
		else
		{
			// Generate leaf
            UStaticMesh* RandomLeafMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Grape/leaves"));

            UStaticMeshComponent* LeafMesh = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("LeafMesh_%d"), i)));
            LeafMesh->SetMobility(EComponentMobility::Movable);
            LeafMesh->AttachToComponent(PetioleSpline, FAttachmentTransformRules::KeepRelativeTransform);
            LeafMesh->RegisterComponent();
            LeafMesh->SetStaticMesh(RandomLeafMesh);

            FTransform LocalTransform;
            FVector LeafPosition = SplinePoint.Position;
            LocalTransform.SetLocation(LeafPosition);
            float RandomScale = FMath::RandRange(0.8f * 0.4f, 1.2f * 0.4f);
            LocalTransform.SetScale3D(FVector(RandomScale));

            //FVector AttachmentPointOnPetiole = SplinePoint.Position;
            //FVector DirectionToRachis = (AttachmentPointOnPetiole - LeafPosition).GetSafeNormal();
            //FQuat TargetQuat = FQuat::FindBetweenVectors(FVector(0, 0, 1), DirectionToRachis);

            // Add a small random twist around the stem direction (local Z)
            float RandomYaw = FMath::RandRange(-45.f, 45.f);
            FQuat RandomTwistQuat = FQuat(PetioleNormal, FMath::DegreesToRadians(RandomYaw));

            LocalTransform.SetRotation(RandomTwistQuat * PetioleRotation);

            LeafMesh->SetRelativeTransform(LocalTransform);
            LeafMeshes.Add(LeafMesh);
		}
	}
}
