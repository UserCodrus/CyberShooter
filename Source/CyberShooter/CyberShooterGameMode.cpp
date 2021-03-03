// Copyright Epic Games, Inc. All Rights Reserved.

#include "CyberShooterGameMode.h"
#include "CyberShooterPawn.h"
#include "CyberShooterGameInstance.h"

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