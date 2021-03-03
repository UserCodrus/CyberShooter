// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterGameInstance.h"
#include "CyberShooterSave.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"

UCyberShooterGameInstance::UCyberShooterGameInstance()
{
	SaveName = "SaveSlot";
	SaveSlot = 0;
	SaveGame = nullptr;

	Gravity = 1000.0f;
	AirFriction = 0.5f;
}

void UCyberShooterGameInstance::LoadGame()
{
	// Copy save data from the currently selected file
	SaveGame = Cast<UCyberShooterSave>(UGameplayStatics::LoadGameFromSlot(SaveName, SaveSlot));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Loading level:" + SaveGame->CurrentLevel);

	// Switch levels
	UGameplayStatics::OpenLevel(GetWorld(), FName(SaveGame->CurrentLevel));
}

void UCyberShooterGameInstance::SaveCheckpoint(FString Level, FVector Location)
{
	// Create a new save game if needed
	CreateNewSave();

	// Store the checkpoint
	SaveGame->CurrentLevel = Level;
	SaveGame->Location = Location;

	// Save the game to disk
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveName, SaveSlot);

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Checkpoint saved");
}

void UCyberShooterGameInstance::SavePlayer(int32 MaxHealth, int32 MaxMomentum, int32 Keys, TArray<class UWeapon*> Weapons, TArray<class UAbility*> Abilities)
{
	// Create a new save game if needed
	if (!CreateNewSave())
	{
		SaveGame->MaxHealth = MaxHealth;
		SaveGame->MaxMomentum = MaxMomentum;
		SaveGame->TotalKeys = Keys;

		SaveGame->Weapons = Weapons;
		SaveGame->Abilities = Abilities;

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Player data saved");
	}
}

UAbilityScript* UCyberShooterGameInstance::GetScript(TSubclassOf<UAbilityScript> ScriptType)
{
	// Check for existing scripts
	for (int32 i = 0; i < Scripts.Num(); ++i)
	{
		if (Scripts[i]->StaticClass() == ScriptType)
		{
			return Scripts[i];
		}
	}

	// Create a new script if one does not exist
	Scripts.Add(NewObject<UAbilityScript>(this, ScriptType));
	return Scripts.Last();
}

bool UCyberShooterGameInstance::CreateNewSave()
{
	if (SaveGame == nullptr)
	{
		// Create the save
		SaveGame = Cast<UCyberShooterSave>(UGameplayStatics::CreateSaveGameObject(UCyberShooterSave::StaticClass()));

		// Load player data
		for (TActorIterator<ACyberShooterPlayer> actor_itr(GetWorld()); actor_itr; ++actor_itr)
		{
			actor_itr->SaveData(SaveGame);
			break;
		}

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "New save data created");
		return true;
	}

	return false;
}