// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CyberShooterSave.generated.h"

// The current status of a given level
USTRUCT()
struct FLevelState
{
	GENERATED_BODY()

	// The name of the level this state corresponds to
	UPROPERTY(Category = "Level", EditAnywhere)
		FString LevelName;

	// Flags for the number of data keys collected
	UPROPERTY(Category = "Collectibles", EditAnywhere)
		TArray<bool> KeyCollected;
	// Flags for each health upgrade the player has collected
	UPROPERTY(Category = "Collectibles", EditAnywhere)
		TArray<bool> HealthUpgradeCollected;
	// A flag for when the player has collected the level's momentum upgrade
	UPROPERTY(Category = "Collectibles", EditAnywhere)
		TArray<bool> MomentumUpgradeCollected;

	// A flag for the level's weapon, if applicable
	UPROPERTY(Category = "Collectibles", EditAnywhere)
		bool WeaponCollected;
	// A flag for the level's ability, if applicable
	UPROPERTY(Category = "Collectibles", EditAnywhere)
		bool AbilityCollected;
};

// Save game data
UCLASS()
class CYBERSHOOTER_API UCyberShooterSave : public USaveGame
{
	GENERATED_BODY()

public:
	// The name of the current level
	UPROPERTY(Category = "Location", EditAnywhere)
		FString CurrentLevel;
	// The player's current location in the level
	UPROPERTY(Category = "Location", EditAnywhere)
		FVector Location;

	// The player's current max health
	UPROPERTY(Category = "Player", EditAnywhere)
		int32 MaxHealth;
	// The player's current max momentum
	UPROPERTY(Category = "Player", EditAnywhere)
		int32 MaxMomentum;
	// The number of keys the player has collected
	UPROPERTY(Category = "Player", EditAnywhere)
		int32 TotalKeys;

	// The weapons the player has obtained
	UPROPERTY(Category = "Player", EditAnywhere)
		TArray<class UWeapon*> Weapons;
	// The abilities the player has obtained
	UPROPERTY(Category = "Player", EditAnywhere)
		TArray<class UAbility*> Abilities;

	// The progress made in each individual level
	UPROPERTY(Category = "Levels", EditAnywhere)
		TArray<FLevelState> LevelStatus;
};
