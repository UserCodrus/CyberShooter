// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Lock.h"
#include "LockInterface.h"

#include "Kismet/GameplayStatics.h"

ALock::ALock()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set defaults
	NumTriggers = 0;
	RequiredTriggers = 1;
	PermanentUnlock = false;
	Locked = true;
}

/// Lock Functions ///

bool ALock::TriggerLock()
{
	NumTriggers++;

	// Unlock the target object if enough triggers have been used
	if (NumTriggers >= RequiredTriggers && Locked)
	{
		Locked = false;
		Unlock();
		return true;
	}

	return false;
}

bool ALock::UntriggerLock()
{
	NumTriggers--;

	// Lock the target object if enough triggers have been used
	if (!PermanentUnlock)
	{
		if (NumTriggers < RequiredTriggers && !Locked)
		{
			Locked = true;
			Lock();
			return true;
		}
	}

	return false;
}

void ALock::Unlock()
{
	// Unlock the targets
	for (int32 i = 0; i < Targets.Num(); ++i)
	{
		ILockInterface* lock_target = Cast<ILockInterface>(Targets[i]);
		if (lock_target != nullptr)
		{
			lock_target->Unlock();
		}
	}

	// Play the unlock sound
	if (UnlockSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, UnlockSound, GetActorLocation());
	}

	// Call blueprint events
	OnUnlock();
}

void ALock::Lock()
{
	// Lock the targets
	for (int32 i = 0; i < Targets.Num(); ++i)
	{
		ILockInterface* lock_target = Cast<ILockInterface>(Targets[i]);
		if (lock_target != nullptr)
		{
			lock_target->Lock();
		}
	}

	// Play the Lock sound
	if (LockSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LockSound, GetActorLocation());
	}

	// Call blueprint events
	OnLock();
}