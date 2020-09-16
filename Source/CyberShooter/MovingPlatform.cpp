// Copyright © 2020 Brian Faubion. All rights reserved.

#include "MovingPlatform.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Engine.h"

AMovingPlatform::AMovingPlatform()
{
	// Enable ticking to apply velocity every frame
	PrimaryActorTick.bCanEverTick = true;

	// Adjust mesh settings for physics
	UStaticMeshComponent* mesh = GetStaticMeshComponent();
	mesh->SetMobility(EComponentMobility::Movable);

	DestinationMarker = CreateDefaultSubobject<USceneComponent>(TEXT("DestinationMarkerComponent"));
	DestinationMarker->bVisualizeComponent = true;
	DestinationMarker->SetAbsolute(true, true, true);
	DestinationMarker->SetupAttachment(RootComponent);

	Origin = FVector(0.0f, 0.0f, 0.0f);
	TravelTime = 10.0f;
	DelayTime = 1.0f;

	Speed = 0.0f;
	Timer = -1.0f;
	Delayed = true;
	ReturnTrip = true;
	Automated = true;
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// Start the platform's movement cycle
	Origin = GetActorLocation();

	// Calculate the speed for velocity calculations later
	Speed = FVector::Distance(Origin, DestinationMarker->GetComponentLocation()) / TravelTime;
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Manage the platform timer
	if (Timer > 0.0f)
	{
		Timer -= DeltaTime;
	}
	if (Timer <= 0.0f)
	{
		if (Delayed)
		{
			if (Automated)
			{
				StartMoving();
			}
		}
		else
		{
			StopMoving();
		}
	}

	if (!Delayed)
	{
		// Change the position of the platform over time
		FVector location;
		if (ReturnTrip)
		{
			location = Origin + (DestinationMarker->GetComponentLocation() - Origin) * (Timer / TravelTime);
		}
		else
		{
			location = DestinationMarker->GetComponentLocation() + (Origin - DestinationMarker->GetComponentLocation()) * (Timer / TravelTime);
		}
		SetActorLocation(location);
	}
	else
	{
		// Set the platform to a static position
		if (ReturnTrip)
		{
			SetActorLocation(Origin);
		}
		else
		{
			SetActorLocation(DestinationMarker->GetComponentLocation());
		}
	}
}

FVector AMovingPlatform::GetVelocity() const
{
	if (Delayed)
	{
		return FVector(0.0f);
	}
	else
	{
		if (ReturnTrip)
		{
			return (Origin - DestinationMarker->GetComponentLocation()).GetSafeNormal() * Speed;
		}
		else
		{
			return (DestinationMarker->GetComponentLocation() - Origin).GetSafeNormal() * Speed;
		}
	}
}

void AMovingPlatform::ResetMarker()
{
	DestinationMarker->SetWorldLocation(GetActorLocation());
}

bool AMovingPlatform::ActivatePlatform()
{
	if (State == EPlatformState::MOVING)
	{
		return false;
	}
	else
	{
		StartMoving();
		return true;
	}
}

void AMovingPlatform::StartMoving()
{
	Timer = TravelTime;
	Delayed = false;
	ReturnTrip = !ReturnTrip;

	State = EPlatformState::MOVING;
}

void AMovingPlatform::StopMoving()
{
	Timer = DelayTime;
	Delayed = true;

	if (ReturnTrip)
	{
		State = EPlatformState::WAITING_AT_ORIGIN;
	}
	else
	{
		State = EPlatformState::WAITING_AT_DESTINATION;
	}
}