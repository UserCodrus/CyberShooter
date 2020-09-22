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

	EnemyPawn = Cast<AEnemyTurret>(GetPawn());
	Player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AEnemyTurretAI::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (EnemyPawn != nullptr && Player != nullptr)
	{
		// Determine if the player is within range of the turret
		FVector aim = Player->GetActorLocation() - EnemyPawn->GetActorLocation();
		if (aim.SizeSquared() > EnemyPawn->GetAggroDistance() * EnemyPawn->GetAggroDistance())
		{
			EnemyPawn->StopFiring();
		}
		else
		{
			// Aim the turret at the player
			if (EnemyPawn->IsTargetingConstrained())
			{

			}
			else
			{
				// Make sure the turret can see the player
				FCollisionQueryParams params;
				params.AddIgnoredActor(EnemyPawn);
				params.AddIgnoredActor(Player);

				FHitResult hit;
				if (!GetWorld()->LineTraceSingleByChannel(hit, EnemyPawn->GetActorLocation(), Player->GetActorLocation(), ECollisionChannel::ECC_Visibility))
				{
					// Rotate the turret to face the player
					float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(aim.GetUnsafeNormal(), EnemyPawn->GetActorForwardVector())));
					float max_angle = EnemyPawn->GetRotationSpeed() * DeltaSeconds;
					if (angle > max_angle)
					{
						EnemyPawn->SetActorRotation(FQuat::Slerp(EnemyPawn->GetActorRotation().Quaternion(), aim.Rotation().Quaternion(), max_angle / angle));
					}
					else
					{
						EnemyPawn->SetActorRotation(aim.Rotation());
					}

					// Fire the turret when it is close to aiming at the player
					if (angle < EnemyPawn->GetFiringAngle())
					{
						EnemyPawn->StartFiring();
					}
					else
					{
						EnemyPawn->StopFiring();
					}
				}
				else
				{
					EnemyPawn->StopFiring();
				}
			}
		}
	}
}