// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PhysicsInteraction.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPhysicsInteraction : public UInterface
{
	GENERATED_BODY()
};

// An interface for actors that can interact with the physics system
class CYBERSHOOTER_API IPhysicsInteraction
{
	GENERATED_BODY()

public:
	// Add a velocity impulse to the object
	UFUNCTION()
		virtual void AddImpulse(FVector Force) = 0;
	// Add a velocity impulse relative to the object's frame of reference
	UFUNCTION()
		virtual void AddRelativeImpulse(FVector Force) = 0;
	// Add a static force to the object
	UFUNCTION()
		virtual void AddStaticForce(FVector Force) = 0;
	// Remove a static force to the object
	UFUNCTION()
		virtual void RemoveStaticForce(FVector Force) = 0;
	// Remove all static forces being applied to an object
	UFUNCTION()
		virtual void ResetStaticForce() = 0;

	// Enable or disable gravity
	UFUNCTION()
		virtual void SetGravityEnabled(bool Enable) = 0;
	// Change the gravity applied to an object
	UFUNCTION()
		virtual void SetGravity(float NewGravity) = 0;
	// Reset gravity to the world's default
	UFUNCTION()
		virtual void ResetGravity() = 0;
	// Change air friction
	UFUNCTION()
		virtual void SetAirFriction(float NewFriction) = 0;
	// Reset air friction to the world default
	UFUNCTION()
		virtual void ResetAirFriction() = 0;
	// Set global friction
	UFUNCTION()
		virtual void SetFriction(float NewFriction) = 0;
	// Reset global friction to world default
	UFUNCTION()
		virtual void ResetFriction() = 0;

	// Change the multiplier for ticks
	UFUNCTION()
		virtual void SetTickRate(float NewRate) = 0;
};
