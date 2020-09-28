// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "PhysicsInteraction.h"

#include "CoreMinimal.h"
#include "PhysicalStaticMesh.h"
#include "PhysicsObject.generated.h"

class UPhysicsMovementComponent;

// A static mesh object with physics enabled
UCLASS()
class CYBERSHOOTER_API APhysicsObject : public APhysicalStaticMesh, public IPhysicsInteraction
{
	GENERATED_BODY()
	
public:
	APhysicsObject();

	virtual void BeginPlay() override;

	/// IPhysicsInteraction ///
	
	void AddImpulse(FVector Force);
	void AddRelativeImpulse(FVector Force);
	void AddStaticForce(FVector Force);
	void RemoveStaticForce(FVector Force);
	void ResetStaticForce();
	void SetGravityEnabled(bool Enable);
	void SetGravity(float NewGravity);
	void ResetGravity();
	void SetAirFriction(float NewFriction);
	void ResetAirFriction();
	void SetFriction(float NewFriction);
	void ResetFriction();
	void SetTickRate(float NewRate);

protected:
	/// Properties ///
	
	// The movement component that handles physics
	UPROPERTY(Category = "Components", VisibleAnywhere)
		UPhysicsMovementComponent* MovementComponent;
};
