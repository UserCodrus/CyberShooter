// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

UENUM()
enum class EPlatformState
{
	WAITING_AT_ORIGIN,
	WAITING_AT_DESTINATION,
	MOVING
};

// A moving platform that changes direction after hitting a platform trigger
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AMovingPlatform();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual FVector GetVelocity() const override;

	// Move the marker to the center of the platform
	UFUNCTION(Category = Platform, CallInEditor)
		void ResetMarker();
	// Called to start the platform moving when it is waiting at a destination
	UFUNCTION(Category = Platform, BlueprintCallable)
		bool ActivatePlatform();

protected:
	// Called to start the platform moving
	void StartMoving();
	// Called to stop the platform from moving
	void StopMoving();

	// The marker for the platform's destination
	UPROPERTY(Category = Platform, EditAnywhere)
		USceneComponent* DestinationMarker;
	// The starting point of the platform
	UPROPERTY(Category = Platform, EditAnywhere)
		FVector Origin;
	// The time it takes the platform to move between points
	UPROPERTY(Category = Timing, EditAnywhere)
		float TravelTime;
	// The delay in seconds after reaching an endpoint before the platform starts moving again
	UPROPERTY(Category = Timing, EditAnywhere)
		float DelayTime;
	// If set to true, the platform will move automatically, otherwise it must be triggered by calling ActivatePlatform
	UPROPERTY(Category = Timing, EditAnywhere)
		bool Automated;
	// The current state of the platform
	UPROPERTY(Category = Timing, VisibleAnywhere, BlueprintReadOnly)
		EPlatformState State;

	// The speed the platform moves at
	float Speed;
	// The timer for platform movement
	float Timer;
	// Set to true if the platform is being delayed
	bool Delayed;
	// Set to true when moving to the origin, otherwise the platform moves to the destination marker
	bool ReturnTrip;
};
