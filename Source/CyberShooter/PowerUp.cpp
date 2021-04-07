// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PowerUp.h"
#include "LockInterface.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

APowerUp::APowerUp()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &APowerUp::BeginOverlap);

	Active = true;
	RespawnDuration = 60.0f;
	RestrictOrientation = true;
}

void APowerUp::BeginPlay()
{
	if (!Active)
	{
		Disable();
	}
}

void APowerUp::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!Active)
		return;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Verify the player's orientation
		if (RestrictOrientation)
		{
			if (!player->CheckOrientation(RootComponent->GetUpVector()))
			{
				return;
			}
		}

		// Collect the powerup and start the respawn timer
		Collect(player);
		CollectEvent();
		if (RespawnDuration > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &APowerUp::Respawn, RespawnDuration);
		}

		Disable();

		if (CollectSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CollectSound, GetActorLocation());
		}
	}
}

void APowerUp::Disable()
{
	Active = false;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void APowerUp::Respawn()
{
	// Show the power up
	Active = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	// Call blueprint events
	RespawnEvent();
}

void APowerUp::CollectEvent()
{
	OnCollect.Broadcast();

	// Call parent delegates
	APowerUp* parent = Cast<APowerUp>(GetAttachParentActor());
	if (parent != nullptr)
	{
		parent->CollectEvent();
	}
}

void APowerUp::RespawnEvent()
{
	OnRespawn.Broadcast();

	// Call parent delegates
	APowerUp* parent = Cast<APowerUp>(GetAttachParentActor());
	if (parent != nullptr)
	{
		parent->RespawnEvent();
	}
}