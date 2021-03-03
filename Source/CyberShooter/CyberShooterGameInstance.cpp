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
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Loading level: " + SaveGame->CurrentLevel);

	LocationID = -1;

	// Switch levels
	UGameplayStatics::OpenLevel(GetWorld(), FName(SaveGame->CurrentLevel));
}

void UCyberShooterGameInstance::SaveCheckpoint(FString Level, FVector Location)
{
	// Make sure a save exists
	CreateNewSave();
		
	// Save player data
	SavePlayer();

	// Store the checkpoint
	SaveGame->CurrentLevel = Level;
	SaveGame->Location = Location;

	// Save the game to disk
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveName, SaveSlot);

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Checkpoint saved");
}

void UCyberShooterGameInstance::SavePlayer()
{
	// Make sure save data exists
	CreateNewSave();

	// Save player data
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player != nullptr)
	{
		SaveGame->MaxHealth = player->GetMaxHealth();
		SaveGame->MaxMomentum = player->GetMaxMomentum();
		SaveGame->TotalKeys = player->GetKeys();

		SaveGame->Weapons = player->GetWeaponSet();
		SaveGame->Abilities = player->GetAbilitySet();

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

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "New save data created");
		return true;
	}

	return false;
}