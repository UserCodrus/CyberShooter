// Copyright © 2020 Brian Faubion. All rights reserved.

#include "VelocityTrigger.h"
#include "CyberShooterPlayer.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

/// AVelocityTrigger ///

AVelocityTrigger::AVelocityTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set defaults
	Direction = FVector(1.0f, 0.0f, 0.0f);
	Magnitude = 1000.0f;
	RestrictOrientation = true;
	RequiredUp = FVector(0.0f, 0.0f, 1.0f);
}

void AVelocityTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Direction.IsNearlyZero() || Magnitude == 0.0f)
		return;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Make sure the player has the orientation required by the trigger
		if (RestrictOrientation)
		{
			if (!player->CheckOrientation(RequiredUp))
			{
				return;
			}
		}

		// Apply the velocity boost
		player->AddImpulse(Direction.GetSafeNormal() * Magnitude);
	}
}

/// AVelocityBoxTrigger ///

AVelocityBoxTrigger::AVelocityBoxTrigger()
{
	// Create a collision box to generate overlap events
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("VelocityCollisionComponent"));
	Collision->SetCollisionProfileName("OverlapAll");
	Collision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AVelocityBoxTrigger::BeginOverlap);
	RootComponent = Collision;
}

/// AVelocitySphereTrigger ///

AVelocitySphereTrigger::AVelocitySphereTrigger()
{
	// Create a collision sphere to generate overlap events
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("VelocityCollisionComponent"));
	Collision->SetCollisionProfileName("OverlapAll");
	Collision->SetSphereRadius(50.0f);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AVelocitySphereTrigger::BeginOverlap);
	RootComponent = Collision;
}