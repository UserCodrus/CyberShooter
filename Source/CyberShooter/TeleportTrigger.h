// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportTrigger.generated.h"

class ACyberShooterPlayer;
#if WITH_EDITORONLY_DATA
	class UArrowComponent;
#endif

// The base class for all teleporters and teleport destinations
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ATeleportBase : public AActor
{
	GENERATED_BODY()

public:
	ATeleportBase();

	// Change the orientation of a player to match this component
	void OrientPlayer(ACyberShooterPlayer* Player);

protected:
	// If set to true, the player's orientation will change after teleporting
	UPROPERTY(Category = "TeleportTarget", EditAnywhere)
		bool SetOrientation;

	// The component that controls the orientation
	UPROPERTY(Category = "Components", EditAnywhere)
		USceneComponent* OrientationComponent;

#if WITH_EDITORONLY_DATA
	// Arrow indicating the forward orientation vector
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* OrientationForwardArrow;
	// Arrow indicating the up orientation vector
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* OrientationUpArrow;
#endif
};

// A trigger that teleports the player to another trigger or a teleport target
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ATeleportTrigger : public ATeleportBase
{
	GENERATED_BODY()
	
public:	
	ATeleportTrigger();

protected:
	// Teleport the player when it overlaps the trigger
	UFUNCTION()
		virtual void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// The teleport target that the player will teleport to
	UPROPERTY(Category = "Teleporter", EditInstanceOnly)
		ATeleportBase* Target;
	// If set to true the player will require the provided up vector to use this teleporter
	UPROPERTY(Category = "Teleporter", EditAnywhere)
		bool RestrictOrientation;

	// The component that controls the required orientation
	UPROPERTY(Category = "Components", EditAnywhere)
		USceneComponent* RequiredOrientationComponent;
	
#if WITH_EDITORONLY_DATA
	// Arrow indicating the required orientation
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* RequiredArrow;
#endif
};