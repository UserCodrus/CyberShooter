// Copyright © 2020 Brian Faubion. All rights reserved.

#include "EnemyTurretAI.h"
#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

AEnemyTurretAI::AEnemyTurretAI()
{
	EnemyPawn = nullptr;
}

void AEnemyTurretAI::BeginPlay()
{
	Super::BeginPlay();

	EnemyPawn = Cast<ACyberShooterEnemy>(GetPawn());
}

void AEnemyTurretAI::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (EnemyPawn != nullptr && player != nullptr)
	{
		// Turn towards the player
		FVector aim = player->GetActorLocation() - EnemyPawn->GetActorLocation();
		EnemyPawn->SetActorRotation(aim.Rotation());

		// Check the distance between the player and the enemy and fire
		if (aim.SizeSquared() > EnemyPawn->GetAggroDistance() * EnemyPawn->GetAggroDistance())
		{
			EnemyPawn->StopFiring();
		}
		else
		{
			EnemyPawn->StartFiring();
		}
	}
}