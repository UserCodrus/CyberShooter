// Copyright © 2020 Brian Faubion. All rights reserved.

#include "MovingPlatform.h"
#include "SplineMovementComponent.h"

#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"

AMovingPlatform::AMovingPlatform()
{
	// Enable ticking to apply velocity every frame
	PrimaryActorTick.bCanEverTick = true;

	// Adjust mesh settings for physics
	UStaticMeshComponent* mesh = GetStaticMeshComponent();
	mesh->SetMobility(EComponentMobility::Movable);

	// Create the spline and movement components
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	Spline->SetupAttachment(RootComponent);
	Spline->SetAbsolute(true, false, true);
	Spline->Duration = 10.0f;

	MovementComponent = CreateDefaultSubobject<USplineMovementComponent>(TEXT("SplineMovementComponent"));
	MovementComponent->Spline = Spline;

	ActiveWhenUnlocked = false;
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (ActiveWhenUnlocked)
	{
		MovementComponent->CanMove = false;
	}
}

FVector AMovingPlatform::GetVelocity() const
{
	return MovementComponent->Velocity;
}

bool AMovingPlatform::IsStable() const
{
	return false;
}

/// ILockInterface ///

void AMovingPlatform::Unlock()
{
	// Activate the platform
	if (ActiveWhenUnlocked)
	{
		MovementComponent->CanMove = true;
	}

	// Start the platform if it isn't already moving
	MovementComponent->StartMovement();

	Execute_OnUnlock(this);
}

void AMovingPlatform::Lock()
{
	// Deactivate the platform
	if (ActiveWhenUnlocked)
	{
		MovementComponent->CanMove = false;
	}

	Execute_OnLock(this);
}

/// Moving Platform Functions ///

bool AMovingPlatform::ActivatePlatform()
{
	// Start the platform
	return MovementComponent->StartMovement();
}

void AMovingPlatform::MovePlatformToStart()
{
	SetActorLocation(Spline->GetComponentLocation());
}

void AMovingPlatform::ResetSpline()
{
	// Remove extra points
	for (int32 i = Spline->GetNumberOfSplinePoints() - 1; i > 0; --i)
	{
		Spline->RemoveSplinePoint(i);
	}

	// Reset the location of the spline
	Spline->SetWorldLocation(GetActorLocation());
}

void AMovingPlatform::MoveSplineToPlatform()
{
	Spline->SetWorldLocation(GetActorLocation());
}

void AMovingPlatform::AppendSpline()
{
	Spline->AddSplineWorldPoint(GetActorLocation());
}