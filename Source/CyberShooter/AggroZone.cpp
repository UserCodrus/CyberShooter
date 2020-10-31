// Copyright © 2020 Brian Faubion. All rights reserved.

#include "AggroZone.h"

#include "CyberShooterPlayer.h"
#include "CyberShooterEnemy.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AAggroZone::AAggroZone()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &AAggroZone::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AAggroZone::EndOverlap);

	// Set up the overlap box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName("OverlapAll");
	CollisionBox->SetBoxExtent(FVector(50.0f));
	RootComponent = CollisionBox;

	Active = true;
	RespawnEnemies = false;
	RespawnTime = 60.0f;
}

void AAggroZone::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!Active)
		return;
	
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Aggro enemies when the player enters the zone
		for (int32 i = 0; i < Enemies.Num(); ++i)
		{
			if (Enemies[i] != nullptr)
			{
				Enemies[i]->Aggro();
			}
		}

		// Cancel the respawn timer
		if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_RespawnTimer) > 0.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RespawnTimer);
		}
	}
	else if (RespawnEnemies)
	{
		// Cancel respawns if the enemy returns to the zone after leaving
		AEnemyBase* enemy = Cast<AEnemyBase>(OtherActor);
		if (enemy != nullptr)
		{
			for (int32 i = 0; i < Enemies.Num(); ++i)
			{
				if (Enemies[i] == enemy)
				{
					enemy->CancelRespawn();
					return;
				}
			}
		}
	}
}

void AAggroZone::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!Active)
		return;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Lose aggro when the player leaves the zone
		for (int32 i = 0; i < Enemies.Num(); ++i)
		{
			if (Enemies[i] != nullptr)
			{
				Enemies[i]->EndAggro();
			}
		}

		// Start the respawn timer
		if (RespawnEnemies)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AAggroZone::Respawn, RespawnTime);
		}
	}
	else if (RespawnEnemies)
	{
		// Respawn enemies that leave the zone
		AEnemyBase* enemy = Cast<AEnemyBase>(OtherActor);
		if (enemy != nullptr)
		{
			for (int32 i = 0; i < Enemies.Num(); ++i)
			{
				if (Enemies[i] == enemy)
				{
					enemy->StartRespawn();
					return;
				}
			}
		}
	}
}

void AAggroZone::UpdateEnemyList()
{
	// Get all the enemies the aggro zone overlaps
	TArray<TEnumAsByte<EObjectTypeQuery>> types;
	TArray<AActor*> ignore;
	TArray<AActor*> actors;
	UKismetSystemLibrary::BoxOverlapActors(GetWorld(), CollisionBox->GetComponentLocation(), CollisionBox->GetScaledBoxExtent(), types, AEnemyBase::StaticClass(), ignore, actors);

	// Add overlapped enemies to the zone
	Enemies.Empty();
	for (int32 i = 0; i < actors.Num(); ++i)
	{
		AEnemyBase* enemy = Cast<AEnemyBase>(actors[i]);
		if (enemy != nullptr)
		{
			Enemies.Add(enemy);
		}
	}
}

void AAggroZone::Respawn()
{
	for (int32 i = 0; i < Enemies.Num(); ++i)
	{
		if (Enemies[i] != nullptr)
		{
			// Enable enemies that have been killed and respawn enemies that are still alive
			if (Enemies[i]->IsDisabled())
			{
				Enemies[i]->EnablePawn();
			}
			else
			{
				Enemies[i]->Respawn();
			}
		}
	}
}

/// ILockInterface ///

void AAggroZone::Unlock()
{
	ClearComponentOverlaps();
	Active = true;
	Execute_OnUnlock(this);

	// Update overlaps to activate the aggro zone if the player is inside it
	UpdateOverlaps();
}

void AAggroZone::Lock()
{
	Active = false;
	Execute_OnLock(this);
}