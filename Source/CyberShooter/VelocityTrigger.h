// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "LockInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VelocityTrigger.generated.h"

#if WITH_EDITORONLY_DATA
class UArrowComponent;
#endif

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AVelocityTrigger : public AActor, public ILockInterface
{
	GENERATED_BODY()
	
public:	
	AVelocityTrigger();

	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

protected:
	/// Velocity Trigger Functions ///
	
	// Apply a velocity impulse to the player when they hit the trigger
	UFUNCTION()
		virtual void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/// Properties ///

	// If set to false, the trigger will not activate when an actor hits it
	UPROPERTY(Category = "VelocityTrigger", EditAnywhere)
		bool Active;
	// The strength of the velocity impulse
	UPROPERTY(Category = "VelocityTrigger", EditAnywhere)
		float Magnitude;
	// If set to true the player will require the provided up vector to use this trigger
	UPROPERTY(Category = "VelocityTrigger", EditAnywhere)
		bool RestrictOrientation;

	// The component that controls the direction of the velocity
	UPROPERTY(Category = "Components", EditAnywhere)
		USceneComponent* DirectionComponent;

#if WITH_EDITORONLY_DATA
	// Arrow indicating the direction of the impulse
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* DirectionArrow;
	// Arrow indicating the required orientation
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* OrientationArrow;
#endif
};