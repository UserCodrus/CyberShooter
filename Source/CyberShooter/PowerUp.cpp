// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PowerUp.h"
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
		if (RespawnDuration > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &APowerUp::Respawn, RespawnDuration);
		}

		Active = false;
		SetActorHiddenInGame(true);

		if (CollectSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CollectSound, GetActorLocation());
		}
	}
}

void APowerUp::Respawn()
{
	Active = true;
	SetActorHiddenInGame(false);
}