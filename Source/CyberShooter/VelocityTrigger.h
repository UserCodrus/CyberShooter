// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VelocityTrigger.generated.h"

class UBoxComponent;
class USphereComponent;

UCLASS(Abstract, BlueprintType)
class CYBERSHOOTER_API AVelocityTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AVelocityTrigger();

protected:
	// Apply a velocity impulse to the player when they hit the trigger
	UFUNCTION()
		virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// The direction of the velocity impulse
	UPROPERTY(Category = VelocityTrigger, EditAnywhere)
		FVector Direction;
	// The strength of the velocity impulse
	UPROPERTY(Category = VelocityTrigger, EditAnywhere)
		float Magnitude;

	// If set to true the player will require the provided up vector to use this trigger
	UPROPERTY(Category = VelocityTrigger, EditAnywhere)
		bool RestrictOrientation;
	// The up vector required to use the restricted trigger
	UPROPERTY(Category = VelocityTrigger, EditAnywhere)
		FVector RequiredUp;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AVelocityBoxTrigger : public AVelocityTrigger
{
	GENERATED_BODY()

public:
	AVelocityBoxTrigger();

protected:
	// The collision box used to generate overlaps
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* Collision;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AVelocitySphereTrigger : public AVelocityTrigger
{
	GENERATED_BODY()

public:
	AVelocitySphereTrigger();

protected:
	// The collision box used to generate overlaps
	UPROPERTY(VisibleAnywhere)
		USphereComponent* Collision;
};