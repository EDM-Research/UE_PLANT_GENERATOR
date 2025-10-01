#include "GrapeClusterActor.h"
#include "Util.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

AGrapeClusterActor::AGrapeClusterActor()
{
    PrimaryActorTick.bCanEverTick = false;

    PhysicsRoot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhysicsRoot"));
    SetRootComponent(PhysicsRoot);
    UStaticMesh* DummyMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    PhysicsRoot->SetStaticMesh(DummyMesh);
    PhysicsRoot->SetVisibility(false);
    PhysicsRoot->SetCollisionResponseToAllChannels(ECR_Ignore);

    RachisSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RachisSpline"));
    RachisSpline->AttachToComponent(PhysicsRoot, FAttachmentTransformRules::KeepRelativeTransform);
    RachisSpline->SetMobility(EComponentMobility::Movable); 
}

void AGrapeClusterActor::BeginPlay()
{
    Super::BeginPlay();
}

void AGrapeClusterActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGrapeClusterActor::ClearGrapeCluster()
{
    for (USplineMeshComponent* Mesh : RachisSplineMeshes)
    {
        if (Mesh && Mesh->IsValidLowLevel())
        {
            Mesh->DestroyComponent();
        }
    }
    RachisSplineMeshes.Empty();

    for (USplineComponent* Spline : PedicelSplines)
    {
        if (Spline && Spline->IsValidLowLevel())
        {
            Spline->DestroyComponent();
        }
    }
    PedicelSplines.Empty();

    for (USplineMeshComponent* Mesh : PedicelMeshes)
    {
        if (Mesh && Mesh->IsValidLowLevel())
        {
            Mesh->DestroyComponent();
        }
    }
    PedicelMeshes.Empty();

    for (UStaticMeshComponent* Mesh : GrapeMeshes)
    {
        if (Mesh && Mesh->IsValidLowLevel())
        {
            Mesh->DestroyComponent();
        }
    }
    GrapeMeshes.Empty();

    RachisSpline->ClearSplinePoints(true);
}

void AGrapeClusterActor::GenerateGrapeCluster(TMap<FString, float> parameters)
{
    ClearGrapeCluster();

    const float RachisLength = FMath::RandRange(parameters.FindRef("RachisLengthMin", 500.0f), parameters.FindRef("RachisLengthMax", 1100.0f));
    const float PedicelLengthMin = parameters.FindRef("PedicelLengthMin", 25.);
    const float PedicelLengthMax = parameters.FindRef("PedicelLengthMax", 200.);
    const bool IsPurple = (bool)parameters.FindRef("IsPurple", false);
    const bool PhysicsEnabled = (bool)parameters.FindRef("PhysicsEnabled", false);
    const float GrapeDetachChance = parameters.FindRef("GrapeDetachChance", 0.0f);

    FAttachmentTransformRules WeldRules(EAttachmentRule::KeepWorld, true);
    
    const int RachisSegments = FMath::RandRange( 3.0f, 6.0f);
    const float RachisBendMagnitude = FMath::RandRange( 150.0f, 1000.0f);
    const FVector RachisBendAxis = FVector(
        FMath::RandRange(0.0f, 1.0f),
        FMath::RandRange(0.0f, 1.0f),
        FMath::RandRange(0.0f, 1.0f)
    ).GetSafeNormal();


    TArray<FSplinePoint> TempRachisSplinePoints;
    const FVector RachisStart = FVector::ZeroVector;
    const FVector BaseRachisDirection = FVector(0, 0, -1); 

    for (int i = 0; i < RachisSegments; ++i)
    {
        float Alpha = (float)i / (float)(RachisSegments - 1 + KINDA_SMALL_NUMBER); 
        FVector PointLocation = RachisStart + BaseRachisDirection * RachisLength * Alpha;
        
        FVector BendOffset = RachisBendAxis * RachisBendMagnitude * FMath::RandRange(0.8f, 1.2f); 
        PointLocation += BendOffset;

        PointLocation.X += FMath::RandRange(-20.0f, 20.0f);
        PointLocation.Y += FMath::RandRange(-20.0f, 20.0f);

        TempRachisSplinePoints.Add(FSplinePoint(i, PointLocation));
    }
    
    RachisSpline->ClearSplinePoints();
    RachisSpline->AddPoints(TempRachisSplinePoints, false); 
    RachisSpline->UpdateSpline(); 
    RachisSpline->SetClosedLoop(false);

    UStaticMesh* RachisMesh =  Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Grape/cylinders/beam"));

    // Create SplineMeshComponents for the rachis segments
    for (int i = 0; i < RachisSpline->GetNumberOfSplinePoints() - 1; ++i)
    {
        USplineMeshComponent* RachisSplineMesh = NewObject<USplineMeshComponent>(this, FName(*FString::Printf(TEXT("RachisSplineMesh_%d"), i)));
        RachisSplineMesh->SetMobility(EComponentMobility::Movable);
        RachisSplineMesh->AttachToComponent(PhysicsRoot, FAttachmentTransformRules::KeepRelativeTransform); 
        RachisSplineMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
        RachisSplineMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
        RachisSplineMesh->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
        RachisSplineMesh->RegisterComponent(); 

        FVector RStart, RTangent, REnd, RTangentEnd;
        RachisSpline->GetLocationAndTangentAtSplinePoint(i, RStart, RTangent, ESplineCoordinateSpace::Local);
        RachisSpline->GetLocationAndTangentAtSplinePoint(i + 1, REnd, RTangentEnd, ESplineCoordinateSpace::Local);

        RachisSplineMesh->SetStaticMesh(RachisMesh);
        RachisSplineMesh->SetStartAndEnd(RStart, RTangent, REnd, RTangentEnd);
        RachisSplineMesh->SetForwardAxis(ESplineMeshAxis::X); 
        RachisSplineMeshes.Add(RachisSplineMesh); 
    }

    TArray<FVector> InitialGrapePositions;
    TArray<FVector> InitialConnectionPositions;
    
    const float GoldenAngle = 137.5f; // Phyllotaxis constant

    UStaticMesh* StemMesh = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Grape/cylinders/small_beam"); 

    // Scale number of grapes based on rachis length 
    int NumGrapes = RachisLength * PedicelLengthMax / 1000;

    for (int i = 0; i < NumGrapes; ++i)
    {
        // Distribute attachment points along the rachis using its length
        float RachisLengthAtPoint = FMath::Pow((float)i / (float)(NumGrapes - 1 + KINDA_SMALL_NUMBER), 2.5) * RachisSpline->GetSplineLength();
        
        FVector AttachmentPointOnRachis = RachisSpline->GetLocationAtDistanceAlongSpline(RachisLengthAtPoint, ESplineCoordinateSpace::Local);
        FVector RachisTangent = RachisSpline->GetTangentAtDistanceAlongSpline(RachisLengthAtPoint, ESplineCoordinateSpace::Local);
        RachisTangent.Normalize();
        
        // Adjust pedicel length based on distance along rachis (shorter at bottom)
        float PedicelLengthRatio = 1.0f - (RachisLengthAtPoint / RachisSpline->GetSplineLength()); 
        float PedicelLength = FMath::RandRange(PedicelLengthMin, PedicelLengthMax) * (0.5f + PedicelLengthRatio * 1.5f); 

        float RadialAngle = (float)i * GoldenAngle;
        
        // Generate a perpendicular vector to the RachisTangent for radial placement
        FVector OrthoPerpendicular = FVector::CrossProduct(RachisTangent, FVector(0,0,1));
        if (OrthoPerpendicular.IsNearlyZero()) 
        {
            OrthoPerpendicular = FVector::CrossProduct(RachisTangent, FVector(0,1,0));
        }
        OrthoPerpendicular.Normalize();

        // Rotate the perpendicular vector around the RachisTangent by RadialAngle
        FVector RotatedOrtho = OrthoPerpendicular.RotateAngleAxis(RadialAngle, RachisTangent);
        
        // Tilt the pedicel direction downwards and slightly outwards
        float DownTiltWeight = FMath::Lerp(0.8f, 0.5f, RachisLengthAtPoint / RachisSpline->GetSplineLength()); 
        
        FVector PedicelDir = FMath::Lerp(FVector(0,0,-1), RotatedOrtho , DownTiltWeight).GetSafeNormal();

        // Add a small cone variation 
        float ConeHalfAngle = 10.0f; 
        PedicelDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(PedicelDir, ConeHalfAngle);

        // Calculate initial grape position
        FVector GrapePos = AttachmentPointOnRachis + PedicelDir * PedicelLength;
        InitialGrapePositions.Add(GrapePos);
        InitialConnectionPositions.Add(AttachmentPointOnRachis);

        // Create spline for pedicel
        USplineComponent* PedicelSpline = NewObject<USplineComponent>(this, FName(*FString::Printf(TEXT("PedicelSpline_%d"), i)));
        PedicelSpline->SetMobility(EComponentMobility::Movable);
        PedicelSpline->AttachToComponent(PhysicsRoot, FAttachmentTransformRules::KeepRelativeTransform); 
        PedicelSpline->SetCollisionResponseToAllChannels(ECR_Ignore);
        PedicelSpline->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
        PedicelSpline->RegisterComponent();
        PedicelSplines.Add(PedicelSpline);

        // Pedicel spline goes from grape to attachment point on rachis
        PedicelSpline->SetSplinePoints({GrapePos, AttachmentPointOnRachis}, ESplineCoordinateSpace::Local);
        PedicelSpline->SetClosedLoop(false);

        // Create a spline mesh component for the pedicel
        USplineMeshComponent* PedicelMesh = NewObject<USplineMeshComponent>(this, FName(*FString::Printf(TEXT("PedicelMesh_%d"), i)));
        PedicelMesh->SetMobility(EComponentMobility::Movable);
        PedicelMesh->AttachToComponent(PhysicsRoot, FAttachmentTransformRules::KeepRelativeTransform); 
        PedicelMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
        PedicelMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
        PedicelMesh->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
        PedicelMesh->RegisterComponent();
        PedicelMeshes.Add(PedicelMesh); 
        
        FVector PedicelStartPos, PedicelStartTangent, PedicelEndPos, PedicelEndTangent;
        PedicelSpline->GetLocationAndTangentAtSplinePoint(0, PedicelStartPos, PedicelStartTangent, ESplineCoordinateSpace::Local);
        PedicelSpline->GetLocationAndTangentAtSplinePoint(1, PedicelEndPos, PedicelEndTangent, ESplineCoordinateSpace::Local);

        PedicelMesh->SetStaticMesh(StemMesh);
        PedicelMesh->SetStartAndEnd(PedicelStartPos, PedicelStartTangent, PedicelEndPos, PedicelEndTangent);
        PedicelMesh->SetForwardAxis(ESplineMeshAxis::X); 
    }
    
    // Apply random hue, saturation, value
    float BaseHue = IsPurple ? 0.0f : 0.2f; 
    float BaseValue = 0.0f;
    float BaseSaturation = 0.f;

    for (int i = 0; i < InitialGrapePositions.Num(); i++)
    {
        UStaticMesh* RandomGrapeMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Grape/")); 

        UStaticMeshComponent* GrapeComp = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("Grape_%d"), i)));
        GrapeComp->SetMobility(EComponentMobility::Movable);
        GrapeMeshes.Add(GrapeComp); 

        if (GrapeComp)
        {
            GrapeComp->SetStaticMesh(RandomGrapeMesh);
            GrapeComp->AttachToComponent(PhysicsRoot, FAttachmentTransformRules::KeepRelativeTransform); 
            GrapeComp->SetCollisionResponseToAllChannels(ECR_Block);
            GrapeComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
            GrapeComp->RegisterComponent();

            FTransform LocalTransform;
            FVector GrapePosition = InitialGrapePositions[i];
            LocalTransform.SetLocation(GrapePosition);
            float RandomScale = FMath::RandRange(0.8f, 1.2f); 
            LocalTransform.SetScale3D(FVector(RandomScale));

            FVector AttachmentPointOnRachis = InitialConnectionPositions[i];
            FVector DirectionToRachis = (AttachmentPointOnRachis - GrapePosition).GetSafeNormal();
            FQuat TargetQuat = FQuat::FindBetweenVectors(FVector(0,0,1), DirectionToRachis); 

            // Add a small random twist around the stem direction (local Z)
            float RandomYaw = FMath::RandRange(-45.f, 45.f); 
            FQuat RandomTwistQuat = FQuat(DirectionToRachis, FMath::DegreesToRadians(RandomYaw));
            
            LocalTransform.SetRotation(RandomTwistQuat * TargetQuat); 

            GrapeComp->SetRelativeTransform(LocalTransform);
        }

        // Create a dynamic material instance for individual grape color variation
        UMaterialInterface* BaseMaterial = GrapeComp->GetMaterial(0);
        UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        if (DynMaterial)
        {
            // Apply slight individual variations
            DynMaterial->SetScalarParameterValue(FName("Hue"), BaseHue + FMath::FRandRange(-0.02f, 0.02f));
            DynMaterial->SetScalarParameterValue(FName("Value"), BaseValue + FMath::FRandRange(-0.05f, 0.05f));
            DynMaterial->SetScalarParameterValue(FName("Saturation"), BaseSaturation + FMath::FRandRange(-0.1f, 0.1f));
            GrapeComp->SetMaterial(0, DynMaterial);
        }
    }
    
    FVector TopOfStemWorld = RachisSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
    FVector CurrentRootWorld = PhysicsRoot->GetComponentLocation();
    FVector Offset = TopOfStemWorld - CurrentRootWorld;
    RachisSpline->AddWorldOffset(-Offset);
    
    if (PhysicsEnabled)
    {
        PhysicsRoot->SetSimulatePhysics(true);
        PhysicsRoot->SetEnableGravity(true);

        for (UStaticMeshComponent* GrapeComp : GrapeMeshes){
            
            bool bShouldDetach = FMath::FRand() < GrapeDetachChance;
    
            if (bShouldDetach)
            {
                GrapeComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
                GrapeComp->SetSimulatePhysics(true);
                GrapeComp->SetEnableGravity(true);
            }
        }
    }
}