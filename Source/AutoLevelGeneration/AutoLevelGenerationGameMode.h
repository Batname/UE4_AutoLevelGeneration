// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AutoLevelGenerationGameMode.generated.h"

UCLASS(minimalapi)
class AAutoLevelGenerationGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAutoLevelGenerationGameMode();


private:
	/** Contains all the available transforms of LevelMovePointActors */
	TArray<FTransform> Transforms;

	FString MapName;

	/** Map to spawn */
	FTransform MapTransform;

	/** Spawns the desired map based on the MapName and the MapTransform */
	UFUNCTION()
	void SpawnMap();

public:
	/**
	 * Moves or spawns a map in the base map
	 * @param MapName - the name we want to spawn
	 * @param Position - the index of the Transform array we want to use
	 */
	UFUNCTION(Exec, Category = LevelStreaming)
	void MoveMap(FString StreamMapName, int32 Position);

	virtual void BeginPlay() override;

};



