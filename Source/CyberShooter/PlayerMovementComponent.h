// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PlayerMovementComponent.generated.h"

class UCapsuleComponent;

// The results of a floor collision test
USTRUCT(BlueprintType)
struct CYBERSHOOTER_API FFloor
{
	GENERATED_BODY()

	// Set to true if the floor is walkable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool Walkable;
	// The distance to the floor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Distance;
	// The friction strength of the floor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Friction;
	// The velocity of the floor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector Velocity;
	// The name of the floor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FString FloorName;
	// The result of the floor test
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FHitResult HitResult;

	FFloor() : Walkable(false), Distance(0.0f), Friction(1.0f), Velocity(0.0f, 0.0f, 0.0f), HitResult(1.0f) {};
};

// The main movement component for player controlled pawns
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent))
class CYBERSHOOTER_API UPlayerMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	UPlayerMovementComponent();

	/// UPawnMovementComponent ///

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnTeleported() override;

	// Change the speed multiplier to a new value
	virtual void ChangeSpeed(float NewSpeed);
	// Change the shape of the floor collision
	virtual void ChangeCollision(UCapsuleComponent* NewCollision);

protected:
	// Apply controller input to the component
	virtual void ApplyControlInputToVelocity(float DeltaTime);
	// Apply gravity to the component
	virtual void ApplyGravityToVelocity(float DeltaTime);
	// Find the floor below the pawn
	virtual bool FindFloor(FFloor& Result);

	virtual float GetMaxSpeed() const override;
	virtual float GetAcceleration() const;
	virtual float GetDeceleration() const;

	// The max speed of the pawn
	UPROPERTY(Category = Movement, EditAnywhere)
		float MaxSpeed;
	// The acceleration applied by inputs
	UPROPERTY(Category = Movement, EditAnywhere)
		float Acceleration;
	// The deceleration applied when no input is available
	UPROPERTY(Category = Movement, EditAnywhere)
		float Deceleration;

	// A boost in acceleration when changing direction to make turns more responsive
	UPROPERTY(Category = Movement, EditAnywhere)
		float TurningMultiplier;
	// A boost to acceleration, deceleration and movement speed based on the player's stats
	UPROPERTY(Category = Movement, EditAnywhere)
		float SpeedMultiplier;
	// The maximum angle of a surface that the pawn will be able to climb
	UPROPERTY(Category = Movement, EditAnywhere)
		float MaxIncline;

	// If set to true the pawn will rotate in the direction of its velocity
	UPROPERTY(Category = Movement, EditAnywhere)
		bool FaceVelocity;

	// The collision shape used to detect the floor
	UPROPERTY(Category = Physics, EditAnywhere)
		UCapsuleComponent* Collision;
	// The floor below the pawn
	UPROPERTY(Category = Physics, EditAnywhere)
		FFloor CurrentFloor;
	// The up vector for the pawn
	UPROPERTY(Category = Physics, EditAnywhere)
		FVector Up;
	// The strength of gravity
	UPROPERTY(Category = Physics, EditAnywhere)
		float Gravity;
	// Friction while in the air
	UPROPERTY(Category = Physics, EditAnywhere)
		float AirFriction;
	// The maximum distance the pawn will be from the floor before gravity shuts off
	UPROPERTY(Category = Movement, EditAnywhere)
		float MaxFloorDistance;
	// The maximum acceleration of gravity
	UPROPERTY(Category = Physics, EditAnywhere)
		float TerminalVelocity;

	// Set to true to notify the pawn that it has been teleported
	bool Teleported;
};
