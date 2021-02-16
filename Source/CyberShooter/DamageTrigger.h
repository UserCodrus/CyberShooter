// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "LockInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTrigger.generated.h"

UCLASS()
class CYBERSHOOTER_API ADamageTrigger : public AActor, public ILockInterface
{
	GENERATED_BODY()
	
public:	
	ADamageTrigger();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

	/// Damage Trigger Functions ///

	// Damage actors inside the trigger
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Stop damaging actors that leave the trigger
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:
	// Activate collision
	void ActivateTrigger();
	// Deactivate collision
	void DeactivateTrigger();

	// Called when the trigger activates
	UFUNCTION(BlueprintImplementableEvent)
		void OnActivate();
	// Called when the trigger deactivates
	UFUNCTION(BlueprintImplementableEvent)
		void OnDeactivate();

	// The damage that actors will take when inside the trigger
	UPROPERTY(Category = "Damage", EditAnywhere)
		int32 Damage;
	// The force feedback from impacts
	UPROPERTY(Category = "Damage", EditAnywhere)
		UForceFeedbackEffect* RumbleEffect;

	// Set to true when the trigger is active
	UPROPERTY(Category = "Damage", EditAnywhere)
		bool Active;
	// Actors that are currently inside the trigger
	UPROPERTY(Category = "Damage", VisibleAnywhere)
		TArray<AActor*> Targets;

	// If set to true, the trigger will cycle between the active and inactive state over time
	UPROPERTY(Category = "Timing", EditAnywhere)
		bool AutoCycle;
	// The amount of time in seconds that the trigger will be active
	UPROPERTY(Category = "Timing", EditAnywhere)
		float ActiveDuration;
	// The amount of time in seconds that the trigger will be dormant
	UPROPERTY(Category = "Timing", EditAnywhere)
		float InactiveDuration;
	// The offset that the timer starts at to ensure triggers alternate activity
	UPROPERTY(Category = "Timing", EditAnywhere)
		float TimerOffset;

	// The timer for managing the trigger
	float Timer;
};
