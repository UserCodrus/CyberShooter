// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CombatInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Switch.generated.h"

class ALock;

// A switch that can trigger a lock
UCLASS(Abstract)
class CYBERSHOOTER_API ASwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	ASwitch();

	// Returns true if the switch has been activated
	UFUNCTION(BlueprintPure)
		bool IsTriggered() const { return Triggered; }
	// Called when the switch timer expires after the switch is untriggered
	void TriggerTimeout();

protected:
	/// Blueprint Events ///

	// Called when the switch is activated
	UFUNCTION(BlueprintImplementableEvent)
		void OnTriggered();
	// Called when the switch is deactivated
	UFUNCTION(BlueprintImplementableEvent)
		void OnUntriggered();

	/// Switch Functions ///

	// Determine if a given actor can trigger this switch
	virtual bool CheckTrigger(AActor* TriggeringActor);
	// Activate the switch
	void TriggerSwitch();
	// Deactivate the switch
	void UntriggerSwitch();

	// Trigger a lock when the switch activates
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Reverse a trigger when the switch deactivates
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// The lock that this switch triggers
	UPROPERTY(Category = "Switch|Trigger", EditInstanceOnly)
		ALock* Target;
	// If set to true, the switch will stay on permanently after triggered, ignoring TriggerDuration
	// Damage switches can be toggled on or off if this is set to false, but will still set timers if TriggerDuration is > 0
	UPROPERTY(Category = "Switch|Trigger", EditAnywhere)
		bool PermanentTrigger;
	// The amount of time the switch will stay triggered after being pressed
	// Damage switches will always switch off after this duration unless set to 0, other switches will ignore this if PermanentTigger is true
	UPROPERTY(Category = "Switch|Trigger", EditAnywhere)
		float TriggerDuration;
	// The number of actors on the switch
	UPROPERTY(Category = "Switch|Trigger", VisibleAnywhere)
		int32 NumTriggers;
	// The number of actors that must be on the switch to activate it
	UPROPERTY(Category = "Switch|Trigger", VisibleAnywhere)
		int32 RequiredTriggers;

	// Set to true after the switch has been activated
	UPROPERTY(VisibleAnywhere)
		bool Triggered;

	// The sound played when the switch triggers
	UPROPERTY(Category = "Switch|Sound", EditAnywhere)
		USoundBase* TriggerSound;
	// The sound played when the switch untriggers
	UPROPERTY(Category = "Switch|Sound", EditAnywhere)
		USoundBase* UntriggerSound;

	// The timer handle for the trigger timer
	FTimerHandle TimerHandle_SwitchTimer;
};

// A switch that can be triggered by overlapping a player
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ASwitch_Player : public ASwitch
{
	GENERATED_BODY()

public:
	ASwitch_Player();

protected:
	virtual bool CheckTrigger(AActor* TriggeringActor);
};

// A switch that can be triggered by overlapping any physics object
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ASwitch_Physics : public ASwitch
{
	GENERATED_BODY()

public:
	ASwitch_Physics();

protected:
	virtual bool CheckTrigger(AActor* TriggeringActor);

	// The total weight required to trigger the switch
	UPROPERTY(Category = "Switch|Trigger", EditAnywhere)
		float RequiredWeight;
};

// A switch that is triggered when it takes damage
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ASwitch_Damage : public ASwitch, public ICombatInterface
{
	GENERATED_BODY()

public:
	ASwitch_Damage();

	/// ICombatInterface ///

	virtual void Damage(int32 Value, int32 DamageType, AActor* Source = nullptr, AActor* Origin = nullptr) override;
	virtual void Heal(int32 Value) override;
	virtual void Kill() override;

protected:
	virtual bool CheckTrigger(AActor* TriggeringActor);

	// The amount of damage that must be dealt to trigger the switch
	UPROPERTY(Category = "Switch|Trigger", EditAnywhere)
		float RequiredDamage;
	// Damage types that won't trigger the switch
	UPROPERTY(Category = "Switch_Trigger", EditAnywhere, meta = (Bitmask, BitmaskEnum = EDamageTypeFlag))
		int32 DamageImmunity;
};