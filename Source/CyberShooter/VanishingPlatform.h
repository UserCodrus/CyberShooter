// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "LockInterface.h"

#include "CoreMinimal.h"
#include "PhysicalStaticMesh.h"
#include "VanishingPlatform.generated.h"

// A platform that appears and disappears
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AVanishingPlatform : public APhysicalStaticMesh, public ILockInterface
{
	GENERATED_BODY()
	
public:
	AVanishingPlatform();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual bool IsStable() const override;

	// Manually change the state of the plaform if it isn't on a timer
	UFUNCTION(BlueprintCallable)
		void ActivatePlatform();

	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

protected:
	/// Vanishing Platform Functions ///
	
	// Called to make the platform swap between visible and hidden states
	void Change();

	// Set to true when the platform is solid and false when it disappears
	UPROPERTY(Category = "VanishingPlatform|State", EditAnywhere)
		bool Solid;
	// If set to false, the platform won't count down its timer or change states
	UPROPERTY(Category = "VanishingPlatform|State", EditAnywhere)
		bool Active;
	// If set to true, the platform's vanish timer won't start until ActivatePlatform is called
	UPROPERTY(Category = "VanishingPlatform|State", EditAnywhere)
		bool ManualActivation;

	// The length of time that the platform will be visible before vanishing
	// If set to zero the platform will stay visible permanently unless activated
	UPROPERTY(Category = "VanishingPlatform|Timing", EditAnywhere)
		float VanishDuration;
	// The length of time that the platform will be hidden
	// If set to zero the platform will stay hidden permanently unless activated
	UPROPERTY(Category = "VanishingPlatform|Timing", EditAnywhere)
		float RevealDuration;
	// The amount of time that the platform will blink when vanishing
	// If set to zero the platform will never blink
	UPROPERTY(Category = "VanishingPlatform|Timing", EditAnywhere)
		float AlertDuration;

	// The rate at which the platform blinks when duration is low, in blinks per second
	UPROPERTY(Category = "VanishingPlatform|Settings", EditAnywhere)
		int32 BlinkSpeed;
	// Time removed from the timer when gameplay starts, in order to make vanishing platforms operate out of phase
	UPROPERTY(Category = "VanishingPlatform|Settings", EditAnywhere)
		float TimerOffset;

	// The timer that tracks the state of the platform
	float Timer;
};
