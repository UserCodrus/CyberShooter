// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsZone.h"
#include "CyberShooterPlayer.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

APhysicsZone::APhysicsZone()
{
	PrimaryActorTick.bCanEverTick = false;

	AllowGravity = true;
	Gravity = 1000.0f;

	SetOrientation = false;
	Forward = FVector(1.0f, 0.0f, 0.0f);
	Up = FVector(0.0f, 0.0f, 1.0f);

	Friction = 1.0f;
	AirFriction = 0.5f;
	Force = FVector(0.0f);
	TickRate = 1.0f;

	OnActorBeginOverlap.AddDynamic(this, &APhysicsZone::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &APhysicsZone::EndOverlap);
}

void APhysicsZone::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Set player attributes
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		if (SetOrientation)
		{
			player->SetOrientation(Forward, Up);
		}

		player->SetWorldGravity(Gravity);
		player->SetGravityEnabled(AllowGravity);

		player->SetFriction(Friction);
		player->SetAirFriction(AirFriction);
		player->SetStaticForce(Force);
	}
}

void APhysicsZone::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Reset player attributes
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		if (SetOrientation)
		{
			player->RevertOrientation();
		}

		player->ResetWorldGravity();
		player->SetGravityEnabled(true);

		player->ResetFriction();
		player->ResetAirFriction();
		player->ResetStaticForce();
	}
}

APhysicsZoneBox::APhysicsZoneBox()
{
	// Create a collision box
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneCollisionComponent"));
	Collision->SetCollisionProfileName("OverlapAll");
	Collision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	RootComponent = Collision;
}

APhysicsZoneSphere::APhysicsZoneSphere()
{
	// Create a collision sphere
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneCollisionComponent"));
	Collision->SetCollisionProfileName("OverlapAll");
	Collision->SetSphereRadius(50.0f);
	RootComponent = Collision;
}