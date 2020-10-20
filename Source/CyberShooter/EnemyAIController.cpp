// Copyright © 2020 Brian Faubion. All rights reserved.

#include "EnemyAIController.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterEnemy.h"

#include "Kismet/GameplayStatics.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference to the player and enemy
	Player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}