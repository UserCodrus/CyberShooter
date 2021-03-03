// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "Ability.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CyberShooterGameInstance.generated.h"

// The game instance
UCLASS(Blueprintable)
class CYBERSHOOTER_API UCyberShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UCyberShooterGameInstance();

	/// Accessors ///
	
	// Load the current save data
	void LoadGame();

	// Save the current checkpoint and then save data to a file
	void SaveCheckpoint(FString Level, FVector Location);
	// Save the player's status
	void SavePlayer(int32 MaxHealth, int32 MaxMomentum, int32 Keys, TArray<class UWeapon*> Weapons, TArray<class UAbility*> Abilities);

	// Retrieve an ability script
	UAbilityScript* GetScript(TSubclassOf<UAbilityScript> ScriptType);

	inline const class UCyberShooterSave* GetSaveData() const { return SaveGame; };
	inline float GetGravity() const { return Gravity; }
	inline float GetAirFriction() const { return AirFriction; }

protected:
	// Create new save data to store information
	bool CreateNewSave();

	// The save slot name
	UPROPERTY(Category = "Save Data", EditDefaultsOnly)
		FString SaveName;
	// The current save slot
	UPROPERTY(Category = "Save Data", EditInstanceOnly)
		int32 SaveSlot;
	// The current save state of the game
	UPROPERTY(Category = "Save Data", EditInstanceOnly)
		class UCyberShooterSave* SaveGame;

	// The current world gravity
	UPROPERTY(Category = "Physics", EditDefaultsOnly)
		float Gravity;
	// The current world air friction
	UPROPERTY(Category = "Physics", EditDefaultsOnly)
		float AirFriction;

	// The ability scripts used by the ability system
	UPROPERTY(Category = "Scripts", VisibleAnywhere)
		TArray<UAbilityScript*> Scripts;
};
