// Copyright © 2020 Brian Faubion. All rights reserved.

#include "TeleportTrigger.h"
#include "CyberShooterPlayer.h"

#include "Components/BoxComponent.h"

/// ATeleportBase ///

ATeleportBase::ATeleportBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SetOrientation = true;
	Forward = FVector(1.0f, 0.0f, 0.0f);
	Up = FVector(0.0f, 0.0f, 1.0f);
}

void ATeleportBase::OrientPlayer(ACyberShooterPlayer* Player)
{
	if (SetOrientation)
	{
		// Set the player's orientation to match the component
		Player->SetOrientation(Forward, Up, true);
	}
}

/// ATeleportTrigger ///

ATeleportTrigger::ATeleportTrigger()
{
	Target = nullptr;
	RestrictOrientation = true;
	RequiredUp = FVector(0.0f, 0.0f, 1.0f);
}

void ATeleportTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Target == nullptr)
		return;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Ensure that we didn't just get here from another telporter
		if (!player->CheckTeleport(this))
			return;

		// Make sure the player has the orientation required by the trigger
		if (RestrictOrientation)
		{
			if (!player->GetUpVector().GetSafeNormal().Equals(RequiredUp.GetSafeNormal()))
			{
				return;
			}
		}

		// Teleport the player
		player->Teleport(this, Target);
	}
}

/// ATeleportBoxTrigger ///

ATeleportBoxTrigger::ATeleportBoxTrigger()
{
	// Create a collision box to generate overlap events
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("OrientationCollisionComponent"));
	Collision->SetCollisionProfileName("OverlapAll");
	Collision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATeleportBoxTrigger::BeginOverlap);
	RootComponent = Collision;
}