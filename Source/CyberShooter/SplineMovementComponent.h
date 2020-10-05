// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "SplineMovementComponent.generated.h"

class USplineComponent;

// A movement component that follows a spline
UCLASS()
class CYBERSHOOTER_API USplineMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
	
public:
	USplineMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Start moving the actor when it is waiting at an endpoint
	bool StartMovement();

protected:
	// Start moving the object
	void StartMoving();
	// Stop the object's movement
	void StopMoving();

public:
	/// Properties ///

	// The spline the actor will follow
	UPROPERTY(Category = "Movement", EditAnywhere)
		USplineComponent* Spline;
	// If set to false, the actor will stop moving completely
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool CanMove;

protected:
	// If set to true, the actor will move automatically, otherwise StartMovement must be called to begin moving
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool Automated;
	// If set to true, the actor will make a complete circuit before stopping after activated
	// If Automated is true this is ignored
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool RoundTrip;
	// The delay in seconds after reaching an endpoint before the object starts moving again
	UPROPERTY(Category = "Movement", EditAnywhere)
		float Delay;
	// If set to true, the object will warp back to the first spline point after reaching the end
	// Otherwise it will move back and forth between the two
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool OneWay;
	// Set to false when moving from the starting spline point to the end, and true to move from the end to the start
	// If OneWay is enabled, this will dictate the direction the pawn moves, otherwise the pawn will first move in the opposite direction of this setting
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool Reverse;

	// The timer for movement
	float Timer;
	// Set to true if the actor is waiting at an endpoint
	bool IsDelayed;
};
