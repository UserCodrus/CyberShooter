// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportTrigger.generated.h"

class UBoxComponent;
class USphereComponent;
class ACyberShooterPlayer;
class UOrientationComponent;

// The base class for all teleporters and teleport destinations
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ATeleportBase : public AActor
{
	GENERATED_BODY()

public:
	ATeleportBase();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;

	// Change the orientation of a player to match this component
	void OrientPlayer(ACyberShooterPlayer* Player);

protected:
	// Set the orientation arrows for the editor
	virtual void SetArrows();

	// If set to true, the player's orientation will change after teleporting
	UPROPERTY(Category = TeleportTarget, EditAnywhere)
		bool SetOrientation;
	// The forward vector to give the player when telporting to this trigger
	UPROPERTY(Category = TeleportTarget, EditAnywhere)
		FVector Forward;
	// The up vector to give the player when telporting to this trigger
	UPROPERTY(Category = TeleportTarget, EditAnywhere)
		FVector Up;

	// A helper component for visualizing the current orientation in the editor
	UPROPERTY(VisibleAnywhere)
		UOrientationComponent* OrientationVisualizer;
};

// A trigger that teleports the player to another trigger or a teleport target
UCLASS(Abstract, BlueprintType)
class CYBERSHOOTER_API ATeleportTrigger : public ATeleportBase
{
	GENERATED_BODY()
	
public:	
	ATeleportTrigger();

protected:
	// Teleport the player when it overlaps the trigger
	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Set the orientation arrows for the editor
	virtual void SetArrows() override;

	// The teleport target that the player will teleport to
	UPROPERTY(Category = Teleporter, EditInstanceOnly)
		ATeleportBase* Target;
	// If set to true the player will require the provided up vector to use this teleporter
	UPROPERTY(Category = Teleporter, EditAnywhere)
		bool RestrictOrientation;
	// The up vector required to use the restricted trigger
	UPROPERTY(Category = Teleporter, EditAnywhere)
		FVector RequiredUp;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ATeleportBoxTrigger : public ATeleportTrigger
{
	GENERATED_BODY()

public:
	ATeleportBoxTrigger();

protected:
	// The collision box used to generate overlaps
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* Collision;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ATeleportSphereTrigger : public ATeleportTrigger
{
	GENERATED_BODY()

public:
	ATeleportSphereTrigger();

protected:
	// The collision box used to generate overlaps
	UPROPERTY(VisibleAnywhere)
		USphereComponent* Collision;
};