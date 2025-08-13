#include "Crop_generator.h"
#include "GrapeClusterActor.h"
#include "CarrotActor.h"
#include "CornActor.h"
#include "AppleActor.h"
#include "GrapevineActor.h"

AActor* UCrop_Generator::Create_variation(Plant_types GeneratorType, FTransform Location, TMap<FString, float> parameters)
{
	switch (GeneratorType)
	{
	case Plant_types::Carrot:
		{
			ACarrotActor* NewCarrot = GWorld->SpawnActor<ACarrotActor>(Location.GetLocation(), Location.GetRotation().Rotator());
			if (NewCarrot)
			{
				NewCarrot->GenerateCarrot(parameters); 
			}
			return NewCarrot;
		}
	case Plant_types::Corn:
		{
			ACornActor* NewCorn = GWorld->SpawnActor<ACornActor>(Location.GetLocation(), Location.GetRotation().Rotator());
			if (NewCorn)
			{
				NewCorn->GenerateCorn(parameters); 
			}
			return NewCorn;
		}
	case Plant_types::Grape:
		{
			AGrapeClusterActor* NewGrapeCluster = GWorld->SpawnActor<AGrapeClusterActor>(Location.GetLocation(), Location.GetRotation().Rotator());
			if (NewGrapeCluster)
			{
				NewGrapeCluster->GenerateGrapeCluster(parameters); 
			}
			return NewGrapeCluster; 
		}
	case Plant_types::Apple:
		{
			AAppleActor* Apple = GWorld->SpawnActor<AAppleActor>(Location.GetLocation(), Location.GetRotation().Rotator());
			if (Apple)
			{
				Apple->GenerateApple(parameters); 
			}
			return Apple; 
		}
	case Plant_types::Grapevine:
		{
			AGrapevineActor* Grapevine = GWorld->SpawnActor<AGrapevineActor>(Location.GetLocation(), Location.GetRotation().Rotator());
			if (Grapevine)
			{
				Grapevine->GenerateGrapevine(parameters);
			}
			return Grapevine;
		}
	default:
		return nullptr;
	}
}