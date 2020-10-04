// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "LockInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrientationTrigger.generated.h"

#if WITH_EDITORONLY_DATA
class UArrowComponent;
#endif

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AOrientationTrigger : public AActor, public ILockInterface
{
	GENERATED_BODY()
	
public:	
	AOrientationTrigger();

	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

protected:
	// Set the player's orientation when the player overlaps this trigger
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// If set to false, the trigger will not activate when an actor hits it
	UPROPERTY(Category = "Orientation", EditAnywhere)
		bool Active;
	// If set to true, the trigger will only work if the player's up vector matches the RequiredOrientationComponent
	UPROPERTY(Category = "Orientation", EditAnywhere)
		bool RestrictOrientation;

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
	// Arrow indicating the required orientation
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* RequiredArrow;
#endif
};
