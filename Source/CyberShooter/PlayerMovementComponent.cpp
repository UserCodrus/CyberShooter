// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PlayerMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	MaxSpeed = 1000.0f;
	Acceleration = 2000.0f;
	Deceleration = 4000.0f;

	TurningMultiplier = 5.0f;
	SpeedMultiplier = 1.0f;

	MaxIncline = 45.0f;
	Up = FVector(0.0f, 0.0f, 1.0f);
	Gravity= 1000.0f;
	AirFriction = 1.0f;
	MaxFloorDistance = 5.0f;
	TerminalVelocity = 3000.0f;

	FaceVelocity = false;
	Teleported = false;
}

void UPlayerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Ignore the tick if the component can't move
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	// Reset velocity after being teleported
	if (Teleported)
	{
		Teleported = false;
		Velocity = FVector(0.0f, 0.0f, 0.0f);
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const AController* controller = PawnOwner->GetController();
	if (controller && controller->IsLocalController())
	{
		// Apply controller input to velocity
		if (controller->IsLocalPlayerController() == true)
		{
			ApplyControlInputToVelocity(DeltaTime);
		}

		// Apply gravity
		ApplyGravityToVelocity(DeltaTime);

		// Get the current velocity and move the component
		FVector delta;
		if (CurrentFloor.Walkable)
		{
			delta = (Velocity + CurrentFloor.Velocity) * DeltaTime;
		}
		else
		{
			delta = Velocity * DeltaTime;
		}
		if (!delta.IsNearlyZero())
		{
			FVector oldlocation = UpdatedComponent->GetComponentLocation();
			FQuat rotation = UpdatedComponent->GetComponentQuat();

			// Move the component
			FHitResult hit(1.0f);
			SafeMoveUpdatedComponent(delta, rotation, true, hit);

			// Handle collisions
			if (hit.IsValidBlockingHit())
			{
				HandleImpact(hit, DeltaTime, delta);

				// Calculate the normal of a slope with the maximum angle
				FVector base_step(0.0f, 0.0f, 1.0f);
				FRotator step_delta(MaxIncline, 0.0f, 0.0f);

				// Ensure that the hit normal isn't steeper than the max slope
				FVector new_normal = hit.Normal;
				/*if (FMath::Abs(hit.Normal.Z) < step_delta.RotateVector(base_step).Z)
				{
					new_normal = hit.Normal.GetSafeNormal2D();
				}*/
				SlideAlongSurface(delta, 1.0f - hit.Time, new_normal, hit, true);
			}

			// Update velocity so that position corrections don't cause problems
			//FVector newlocation = UpdatedComponent->GetComponentLocation();
			//Velocity = ((newlocation - oldlocation) / DeltaTime);

			// Rotate the pawn to match velocity
			if (FaceVelocity)
			{
				UpdatedComponent->SetWorldRotation(Velocity.Rotation());
			}
		}

		// Finalize the update
		UpdateComponentVelocity();
	}
}

void UPlayerMovementComponent::OnTeleported()
{
	Super::OnTeleported();

	// Mark the pawn as teleported so that velocity can be reset
	Teleported = true;
}

void UPlayerMovementComponent::ChangeSpeed(float NewSpeed)
{
	if (NewSpeed > 0.0f)
	{
		SpeedMultiplier = NewSpeed;
	}
}

void UPlayerMovementComponent::ChangeCollision(UCapsuleComponent* NewCollision)
{
	Collision = NewCollision;
}

void UPlayerMovementComponent::ApplyControlInputToVelocity(float DeltaTime)
{
	// Get the current input vector
	FVector current_acceleration = GetPendingInputVector().GetClampedToMaxSize(1.0f);
	// Get the horizontal velocity so we can calculate velocity ignoring gravity
	FVector relative_velocity(Velocity.X, Velocity.Y, 0.0f);

	// Get the squared input vector for better velocity scaling
	float analog_modifier = (current_acceleration.SizeSquared() > 0.0f ? current_acceleration.Size() : 0.0f);
	// The max speed of the pawn relative to the input vector
	float current_max_speed = GetMaxSpeed() * analog_modifier;
	// Determine if we are exceeding the current max speed
	bool exceed_max = relative_velocity.SizeSquared() > FMath::Square(current_max_speed) * 1.01f;
	
	if (analog_modifier > 0.0f && !exceed_max)
	{
		// Apply changes in direction
		if (relative_velocity.SizeSquared() > 0.0f)
		{
			float turn_scale = FMath::Clamp(DeltaTime * TurningMultiplier, 0.0f, 1.0f);
			relative_velocity = relative_velocity + (current_acceleration * relative_velocity.Size() - relative_velocity) * turn_scale;
		}
	}
	else
	{
		// Decelerate
		if (relative_velocity.SizeSquared() > 0.0f)
		{
			FVector old_velocity = relative_velocity;
			float magnitude = FMath::Max(relative_velocity.Size() - FMath::Abs(GetDeceleration()) * DeltaTime, 0.f);
			relative_velocity = relative_velocity.GetSafeNormal() * magnitude;

			// Don't brake lower than the max speed
			if (exceed_max && relative_velocity.SizeSquared() < FMath::Square(current_max_speed))
			{
				relative_velocity = relative_velocity.GetSafeNormal() * current_max_speed;
			}
		}
	}

	// Apply current acceleration
	float newspeed = (relative_velocity.SizeSquared() > FMath::Square(current_max_speed) * 1.01f) ? relative_velocity.Size() : current_max_speed;
	relative_velocity += current_acceleration * FMath::Abs(GetAcceleration()) * DeltaTime;
	relative_velocity = relative_velocity.GetClampedToMaxSize(newspeed);

	// Apply changes to velocity
	relative_velocity.Z = Velocity.Z;
	Velocity = relative_velocity;

	ConsumeInputVector();
}

void UPlayerMovementComponent::ApplyGravityToVelocity(float DeltaTime)
{
	// Find the floor below the object
	FFloor newfloor;
	FindFloor(newfloor);
	if (newfloor.HitResult.Actor != nullptr)
	{
		newfloor.HitResult.Actor->GetName(newfloor.FloorName);
		newfloor.Velocity = newfloor.HitResult.Actor->GetVelocity();
	}
	CurrentFloor = newfloor;

	// Apply gravity
	if (!CurrentFloor.Walkable)
	{
		if (Velocity.Z > -TerminalVelocity)
		{
			Velocity.Z -= Gravity * DeltaTime;
		}
		else
		{
			Velocity.Z = -TerminalVelocity;
		}
	}
	else
	{
		Velocity.Z = 0.0f;
	}
}

bool UPlayerMovementComponent::FindFloor(FFloor& Result)
{
	if (Collision == nullptr)
		return false;

	// Get the size of the collision capsule
	float radius, height;
	Collision->GetScaledCapsuleSize(radius, height);

	// Set collision parameters
	FCollisionQueryParams params;
	FCollisionResponseParams response;
	params.AddIgnoredComponent(Cast<UPrimitiveComponent>(UpdatedComponent));
	InitCollisionParams(params, response);
	ECollisionChannel channel = UpdatedComponent->GetCollisionObjectType();

	// Perform a line trace
	float shrink_height = height;
	float trace_distance = MaxFloorDistance + shrink_height;
	FVector trace_start = UpdatedComponent->GetComponentLocation();
	FVector trace_end = trace_start + Up * -trace_distance;
	bool block = GetWorld()->LineTraceSingleByChannel(Result.HitResult, trace_start, trace_end, channel, params, response);

	if (block)
	{
		if (Result.HitResult.Time > 0.0f)
		{
			// Calculate the distance to the hit
			Result.Distance = FMath::Max(-radius, Result.HitResult.Time * trace_distance - shrink_height);
			if (Result.Distance <= MaxFloorDistance)
			{
				Result.Walkable = true;
			}
		}
	}

	return block;
}

float UPlayerMovementComponent::GetMaxSpeed() const
{
	return MaxSpeed * SpeedMultiplier;
}

float UPlayerMovementComponent::GetAcceleration() const
{
	return Acceleration * SpeedMultiplier;
}

float UPlayerMovementComponent::GetDeceleration() const
{
	return Deceleration * SpeedMultiplier;
}