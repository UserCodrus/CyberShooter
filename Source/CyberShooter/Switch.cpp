// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Switch.h"
#include "Lock.h"
#include "CyberShooterPlayer.h"

/// ASwitch ///

ASwitch::ASwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &ASwitch::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ASwitch::EndOverlap);

	PermanentTrigger = true;

	TriggerDuration = 0.0f;
	NumTriggers = 0;
	RequiredTriggers = 1;
}

void ASwitch::TriggerTimeout()
{
	Triggered = false;
	OnUntriggered();
}

/// Blueprint Events ///

void ASwitch::OnTriggered_Implementation()
{
	if (Target != nullptr)
	{
		Target->TriggerLock();
	}
}

void ASwitch::OnUntriggered_Implementation()
{
	if (Target != nullptr)
	{
		Target->UntriggerLock();
	}
}

/// Switch Functions ///

bool ASwitch::CheckTrigger(AActor* TriggeringActor)
{
	return true;
}

void ASwitch::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	NumTriggers++;

	// Unlock the target lock if enough actors are on the switch
	if (NumTriggers >= RequiredTriggers && !Triggered)
	{
		Triggered = true;
		if (TriggerDuration > 0.0f)
		{
			// Cancel the untrigger timer if one is set, otherwise trigger the switch
			if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_SwitchTimer) > 0.0f)
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SwitchTimer);
			}
			else
			{
				OnTriggered();
			}
		}
		else
		{
			// Trigger the switch
			OnTriggered();
		}
	}
}

void ASwitch::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	NumTriggers--;

	// Lock the target lock if too many actors leave the switch
	if (!PermanentTrigger)
	{
		if (NumTriggers < RequiredTriggers && Triggered)
		{
			Triggered = false;
			if (TriggerDuration > 0.0f)
			{
				// Stay triggered for a while
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchTimer, this, &ASwitch::TriggerTimeout, TriggerDuration);
			}
			else
			{
				// Untrigger instantly
				OnUntriggered();
			}
		}
	}
}

/// ASwitch_Player ///

ASwitch_Player::ASwitch_Player()
{

}

bool ASwitch_Player::CheckTrigger(AActor* TriggeringActor)
{
	// Check to see if the tiggering actor is a player
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(TriggeringActor);
	if (player != nullptr)
	{
		return true;
	}

	return false;
}

/// ASwitch_Physics ///

ASwitch_Physics::ASwitch_Physics()
{
	RequiredWeight = 0.0f;
}

bool ASwitch_Physics::CheckTrigger(AActor* TriggeringActor)
{
	// Check to see if the triggering actor is a physics object
	IPhysicsInterface* object = Cast<IPhysicsInterface>(TriggeringActor);
	if (object != nullptr)
	{
		if (object->GetWeight() > RequiredWeight)
		{
			return true;
		}
	}

	return false;
}

/// ASwitch_Damage ///

ASwitch_Damage::ASwitch_Damage()
{
	RequiredDamage = 0;
	DamageImmunity = DAMAGETYPE_NONE;
}

/// ICombatInterface ///

void ASwitch_Damage::Damage(int32 Value, int32 DamageType, AActor* Source, AActor* Origin)
{
	// Make sure the damage is sufficient to trigger the switch
	if (!(DamageImmunity & DamageType) && Value > RequiredDamage)
	{
		if (!Triggered)
		{
			// Trigger the switch
			Triggered = true;
			OnTriggered();

			if (TriggerDuration > 0.0f)
			{
				// Set the trigger timer
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchTimer, this, &ASwitch_Damage::TriggerTimeout, TriggerDuration);
			}
		}
		else if (!PermanentTrigger)
		{
			// Untrigger the switch
			Triggered = false;
			OnUntriggered();

			if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_SwitchTimer) > 0.0f)
			{
				// Cancel the untrigger timer
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SwitchTimer);
			}
		}
		else
		{
			if (TriggerDuration > 0.0f)
			{
				// Reset the timer
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchTimer, this, &ASwitch_Damage::TriggerTimeout, TriggerDuration);
			}
		}
	}
}

void ASwitch_Damage::Heal(int32 Value)
{
	// Do nothing
}

void ASwitch_Damage::Kill()
{
	// Do nothing
}

bool ASwitch_Damage::CheckTrigger(AActor* TriggeringActor)
{
	return false;
}