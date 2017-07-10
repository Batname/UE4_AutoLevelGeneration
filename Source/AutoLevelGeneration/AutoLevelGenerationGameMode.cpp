// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AutoLevelGenerationGameMode.h"
#include "LevelMovePoint.h"
#include "AutoLevelGenerationCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"

AAutoLevelGenerationGameMode::AAutoLevelGenerationGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AAutoLevelGenerationGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> OurActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelMovePoint::StaticClass(), OurActors);

	for (auto It : OurActors)
	{
		// Get transfomr of each Actor;
		Transforms.Add(It->GetTransform());
	}
}

void AAutoLevelGenerationGameMode::SpawnMap()
{

	UE_LOG(LogTemp, Warning, TEXT("AAutoLevelGenerationGameMode::SpawnMap"));

	FLatentActionInfo Info;

	// the reason I've typed *MapName in the parameters is because the second parameters needs an FName so
	// essentially i'm type casting the FString to FName
	UGameplayStatics::LoadStreamLevel(GetWorld(), *MapName, false, false, Info);
	ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(GetWorld(), *MapName);

	// Assign a new transform to our level
	Level->LevelTransform = MapTransform;

	// Make level visible
	Level->bShouldBeVisible = true;
}

void AAutoLevelGenerationGameMode::MoveMap(FString StreamMapName, int32 Position)
{
	if (Transforms.IsValidIndex(Position))
	{

		ULevelStreaming* Level = UGameplayStatics::GetStreamingLevel(GetWorld(), *StreamMapName);

		MapName = StreamMapName;
		MapTransform = Transforms[Position];

		if (Level->IsLevelVisible())
		{
			// if the level is visible we need to upload it first
			// and then spawn into the new location

			// The upload stream level is essentially an async operation
			// Using the latent actor info we're able to create a callbak
			// which is fired when th UnloadStreamLevel finished its execution
			FLatentActionInfo Info;
			Info.CallbackTarget = this;
			Info.ExecutionFunction = FName("SpawnMap");
			Info.UUID = 0;
			Info.Linkage = 0;

			UGameplayStatics::UnloadStreamLevel(GetWorld(), *MapName, Info); 
		}
		else
		{
			// If level is invissible just spawn into world
			SpawnMap();
		}
	}
}