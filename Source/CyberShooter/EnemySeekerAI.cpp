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

			// Move towards the player
			FVector direction = Player->GetActorLocation() - EnemyPawn->GetActorLocation();
			EnemyPawn->AddControlInput(direction);
			EnemyPawn->SetCoreRotation(world_rotation.UnrotateVector(direction).Rotation());
		}
	}
}