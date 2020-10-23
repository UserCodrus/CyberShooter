// Copyright © 2020 Brian Faubion. All rights reserved.

#include "EnemySeekerAI.h"
#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

AEnemySeekerAI::AEnemySeekerAI()
{
	
}

void AEnemySeekerAI::Tick(float DeltaSeconds)
{
	if (EnemyPawn != nullptr && Player != nullptr)
	{
		if (Aggro)
		{
			FRotator world_rotation = EnemyPawn->GetOrientationRotator();

			// Make sure the turret can see the player
			if (CheckLineOfSight())
			{
				// Move towards the player
				//FVector direction = Player->GetActorLocation() - EnemyPawn->GetActorLocation();
				//EnemyPawn->AddControlInput(direction);
				FVector direction = Move();
				
				// Face the player
				FVector aim = world_rotation.UnrotateVector(direction);
				aim.Z = 0.0f;
				EnemyPawn->SetCoreRotation(aim.Rotation());

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