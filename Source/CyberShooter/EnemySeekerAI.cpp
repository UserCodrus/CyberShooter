// Copyright © 2020 Brian Faubion. All rights reserved.

#include "EnemySeekerAI.h"
#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

AEnemySeekerAI::AEnemySeekerAI()
{
	EnemyPawn = nullptr;
}

void AEnemySeekerAI::BeginPlay()
{
	Super::BeginPlay();

	EnemyPawn = Cast<AEnemySeeker>(GetPawn());
}

void AEnemySeekerAI::Tick(float DeltaSeconds)
{
	if (EnemyPawn != nullptr && Player != nullptr)
	{
		if (Aggro)
		{
			FRotator world_rotation = EnemyPawn->GetOrientationRotator();

			// Make sure the turret can see the player
			FCollisionQueryParams params;
			params.AddIgnoredActor(EnemyPawn);
			params.AddIgnoredActor(Player);

			FHitResult hit;
			if (EnemyPawn->CanSeeThroughWalls() || !GetWorld()->LineTraceSingleByChannel(hit, EnemyPawn->GetActorLocation(), Player->GetActorLocation(), ECollisionChannel::ECC_Visibility))
			{
				// Move towards the player
				FVector direction = Player->GetActorLocation() - EnemyPawn->GetActorLocation();
				EnemyPawn->AddControlInput(direction);
				
				FVector aim = world_rotation.UnrotateVector(direction);
				aim.Z = 0.0f;
				EnemyPawn->SetCoreRotation(aim.Rotation());
				EnemyPawn->SetAimRotation(world_rotation.RotateVector(aim).Rotation());

				// Fire the pawn's weapon
				EnemyPawn->StartFiring();
				return;
			}
		}

		// Revert to no firing if any checks fail
		EnemyPawn->StopFiring();
	}
}