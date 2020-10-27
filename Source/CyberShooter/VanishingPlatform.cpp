// Copyright © 2020 Brian Faubion. All rights reserved.

#include "VanishingPlatform.h"

AVanishingPlatform::AVanishingPlatform()
{
	// Enable ticking for timers
	PrimaryActorTick.bCanEverTick = true;

	// Adjust mesh settings
	UStaticMeshComponent* mesh = GetStaticMeshComponent();
	mesh->SetMobility(EComponentMobility::Movable);

	Solid = true;

	VanishDuration = 10.0f;
	RevealDuration = 10.0f;
	TimerOffset = 0.0f;
}

void AVanishingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// Set the initial state
	Solid = !Solid;
	Change();
	Timer -= TimerOffset;
}

void AVanishingPlatform::Tick(float DeltaSeconds)
{
	if (Timer > 0.0f)
	{
		// Manage the timer if it has been set
		Timer -= DeltaSeconds;
		if (Timer <= 0.0f)
		{
			if (Solid)
			{
				Change();
			}
			else
			{
				Change();
			}
		}
	}
}

void AVanishingPlatform::ActivatePlatform()
{
	// Force the platform the change if it isn't timed right now
	if (Timer <= 0.0f)
	{
		// Reset the timer so it doesn't interfere with the forced change
		Timer = 0.0f;
		Change();
	}
}

void AVanishingPlatform::Change()
{
	// Change the state of the platform
	Solid = !Solid;
	SetActorHiddenInGame(!Solid);
	SetActorEnableCollision(Solid);

	// Set the timer if needed
	if (Solid && VanishDuration > 0.0f)
	{
		Timer += VanishDuration;
	}
	else if (!Solid && RevealDuration > 0.0f)
	{
		Timer += RevealDuration;
	}
}

