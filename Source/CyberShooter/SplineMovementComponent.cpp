// Copyright © 2020 Brian Faubion. All rights reserved.

#include "SplineMovementComponent.h"

#include "Components/SplineComponent.h"

USplineMovementComponent::USplineMovementComponent()
{
	// Set defaults
	CanMove = true;
	Spline = nullptr;

	Automated = true;
	RoundTrip = true;
	Delay = 1.0f;
	OneWay = false;
	Reverse = true;

	Timer = 0.0f;
	IsDelayed = true;
}

void USplineMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!CanMove || Spline == nullptr)
		return;

	// Manage the movement timer
	if (Timer > 0.0f)
	{
		Timer -= DeltaTime;
	}
	if (Timer <= 0.0f)
	{
		// Start or stop movement when the timer is up
		if (IsDelayed)
		{
			if (Automated)
			{
				// Always move when automated
				StartMoving();
			}
			else if (RoundTrip && !Reverse)
			{
				// Return to origin if round trip is enabled and we are at the destination
				StartMoving();
			}
		}
		else
		{
			StopMoving();
		}
	}

	if (!IsDelayed)
	{
		float speed = Spline->GetSplineLength() / Spline->Duration;

		// Change the position of the object over time
		if (Reverse)
		{
			UpdatedComponent->SetWorldLocation(Spline->GetWorldLocationAtTime(Timer, true));
			Velocity = Spline->GetWorldDirectionAtTime(Timer, true) * -speed;
		}
		else
		{
			UpdatedComponent->SetWorldLocation(Spline->GetWorldLocationAtTime(Spline->Duration - Timer, true));
			Velocity = Spline->GetWorldDirectionAtTime(Timer, true) * speed;
		}
	}
	else
	{
		// Set the position to a static location
		if (Reverse)
		{
			UpdatedComponent->SetWorldLocation(Spline->GetWorldLocationAtSplinePoint(0));
		}
		else
		{
			UpdatedComponent->SetWorldLocation(Spline->GetWorldLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1));
		}
		Velocity = FVector::ZeroVector;
	}

	UpdateComponentVelocity();
}

bool USplineMovementComponent::StartMovement()
{
	if (IsDelayed)
	{
		StartMoving();
		return true;
	}
	else
	{
		return false;
	}
}

void USplineMovementComponent::StartMoving()
{
	Timer = Spline->Duration;
	IsDelayed = false;

	if (!OneWay)
	{
		Reverse = !Reverse;
	}
}

void USplineMovementComponent::StopMoving()
{
	Timer = Delay;
	IsDelayed = true;
}