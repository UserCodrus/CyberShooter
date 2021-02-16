// Copyright © 2020 Brian Faubion. All rights reserved.

#include "DamageTrigger.h"
#include "CombatInterface.h"

ADamageTrigger::ADamageTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &ADamageTrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ADamageTrigger::EndOverlap);

	// Set property defaults
	Damage = 1;
	
	AutoCycle = true;
	Active = false;
	ActiveDuration = 1.0f;
	InactiveDuration = 1.0f;
	TimerOffset = 0.0f;
}


void ADamageTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AutoCycle)
	{
		// Increment the activity timer
		Timer += DeltaTime;
		if (Active)
		{
			if (Timer > ActiveDuration)
			{
				// Deactivate the trigger
				DeactivateTrigger();
				Timer -= ActiveDuration;
			}
		}
		else
		{
			if (Timer > InactiveDuration)
			{
				// Activate the trigger
				ActivateTrigger();
				Timer -= InactiveDuration;
			}
		}
	}

	// Deal damage to each object inside the trigger
	for (int32 i = 0; i < Targets.Num(); ++i)
	{
		ICombatInterface* object = Cast<ICombatInterface>(Targets[i]);
		if (object != nullptr)
		{
			object->Damage(Damage, EDamageType::DAMAGETYPE_ENVIRONMENT, RumbleEffect, nullptr, this, this);
		}
	}
}

void ADamageTrigger::BeginPlay()
{
	Super::BeginPlay();

	// Set the default timer
	Timer = TimerOffset;

	// Show or hide the actor to match the active state
	SetActorEnableCollision(Active);
}

/// ILockInterface ///

void ADamageTrigger::Unlock()
{
	Active = true;
	Execute_OnUnlock(this);
}

void ADamageTrigger::Lock()
{
	Active = false;
	Execute_OnLock(this);
}

/// Damage Trigger Functions ///

void ADamageTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Add actors to the target list
	ICombatInterface* object = Cast<ICombatInterface>(OtherActor);
	if (object != nullptr)
	{
		Targets.Add(OtherActor);
	}
}

void ADamageTrigger::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Remove actors from the target list
	ICombatInterface* object = Cast<ICombatInterface>(OtherActor);
	if (object != nullptr)
	{
		Targets.Remove(OtherActor);
	}
}

void ADamageTrigger::ActivateTrigger()
{
	Active = true;
	SetActorEnableCollision(true);

	OnActivate();
}

void ADamageTrigger::DeactivateTrigger()
{
	Active = false;
	SetActorEnableCollision(false);

	OnDeactivate();
}