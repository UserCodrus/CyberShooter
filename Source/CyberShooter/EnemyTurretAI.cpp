// Copyright © 2020 Brian Faubion. All rights reserved.

#include "EnemyTurretAI.h"
#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

AEnemyTurretAI::AEnemyTurretAI()
{
	
}

void AEnemyTurretAI::BeginPlay()
{
	Super::BeginPlay();

	AEnemyTurret* turret = Cast<AEnemyTurret>(GetPawn());
	if (turret != nullptr)
	{
		AimSpeed = turret->GetRotationSpeed();
		ConstrainTargeting = turret->IsTargetingConstrained();
		FiringAngle = turret->GetFiringAngle();
	}
}

void AEnemyTurretAI::Tick(float DeltaSeconds)
{
	if (EnemyPawn != nullptr && Player != nullptr)
	{
		if (Aggro)
		{
			// Make sure the turret can see the player
			if (CheckLineOfSight())
			{
				// Fire the turret when it is close to aiming at the player
				if (Aim(DeltaSeconds) <= FiringAngle)
				{
					EnemyPawn->StartFiring();
					return;
				}
			}
		}

		// Revert to no firing if any checks fail
		EnemyPawn->StopFiring();
	}
}