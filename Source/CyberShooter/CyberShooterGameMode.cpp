// Copyright Epic Games, Inc. All Rights Reserved.

#include "CyberShooterGameMode.h"
#include "CyberShooterPawn.h"

ACyberShooterGameMode::ACyberShooterGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = ACyberShooterPawn::StaticClass();
}

