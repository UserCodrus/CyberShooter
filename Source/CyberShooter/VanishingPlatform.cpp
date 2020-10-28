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
	ManualActivation = false;

	VanishDuration = 10.0f;
	RevealDuration = 10.0f;
	AlertDuration = 1.0f;
	BlinkSpeed = 20;
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
		else if (Solid && Timer < AlertDuration)
		{
			// Blink when the timer is low
			int state = (int)(Timer * BlinkSpeed) & 2;
			SetActorHiddenInGame((bool)state);
		}
	}
}

bool AVanishingPlatform::IsStable() const
{
	return false;
}

void AVanishingPlatform::ActivatePlatform()
{
	
	if (Timer <= 0.0f)
	{
		if (ManualActivation)
		{
			// Activate the vanish timer
			if (Solid)
			{
				Timer += VanishDuration;
			}
		}
		else
		{
			// Force the platform the change
			Timer = 0.0f;
			Change();
		}
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
		if (ManualActivation)
		{
			Timer = 0.0f;
		}
		else
		{
			Timer += VanishDuration;
		}
	}
	else if (!Solid && RevealDuration > 0.0f)
	{
		Timer += RevealDuration;
	}
}

