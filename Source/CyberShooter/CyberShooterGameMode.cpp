// Copyright Epic Games, Inc. All Rights Reserved.

#include "CyberShooterGameMode.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterGameInstance.h"
#include "Kismet/GameplayStatics.h"

ACyberShooterGameMode::ACyberShooterGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = ACyberShooterPawn::StaticClass();
}

void ACyberShooterGameMode::LoadGame()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		instance->LoadGame();
	}
}

void ACyberShooterGameMode::LoadLevel(FString LevelName, int32 LocationID)
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		instance->LocationID = LocationID;
		instance->SavePlayer();

		UGameplayStatics::OpenLevel(GetWorld(), FName(LevelName));
	}
}

void ACyberShooterGameMode::RefillPlayer()
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player != nullptr)
	{
		player->Refill();
	}
}