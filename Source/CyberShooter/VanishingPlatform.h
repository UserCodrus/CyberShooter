// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalStaticMesh.h"
#include "VanishingPlatform.generated.h"

// A platform that appears and disappears
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AVanishingPlatform : public APhysicalStaticMesh
{
	GENERATED_BODY()
	
public:
	AVanishingPlatform();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Manually change the state of the plaform if it isn't on a timer
	UFUNCTION(BlueprintCallable)
		void ActivatePlatform();

protected:
	// Called to make the platform swap between visible and hidden states
	void Change();

	// Set to true when the platform is solid and false when it disappears
	UPROPERTY(Category = "VanishingPlatform", EditAnywhere)
		bool Solid;

	// The length of time that the platform will be visible before vanishing
	// If set to zero the platform will stay visible permanently unless activated
	UPROPERTY(Category = "VanishingPlatform", EditAnywhere)
		float VanishDuration;
	// The length of time that the platform will be hidden
	// If set to zero the platform will stay hidden permanently unless activated
	UPROPERTY(Category = "VanishingPlatform", EditAnywhere)
		float RevealDuration;

	// Time removed from the timer when gameplay starts, in order to make vanishing platforms operate out of phase
	UPROPERTY(Category = "VanishingPlatform", EditAnywhere)
		float TimerOffset;

	// The timer that tracks the state of the platform
	UPROPERTY(VisibleAnywhere)
	float Timer;
};
