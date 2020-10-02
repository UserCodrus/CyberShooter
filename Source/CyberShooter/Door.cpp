// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Door.h"

#include "Kismet/GameplayStatics.h"

ADoor::ADoor()
{
	Open = false;
}

/// ILockInterface ///

void ADoor::Unlock()
{
	OpenDoor();
}

void ADoor::Lock()
{
	CloseDoor();
}

/// Door Functions ///

void ADoor::OpenDoor()
{
	// Call the OnOpen event
	if (!Open)
	{
		Open = true;
		OnOpen();

		if (OpenSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
		}
	}
}

void ADoor::CloseDoor()
{
	// Call the OnClose event
	if (Open)
	{
		Open = false;
		OnClose();

		if (CloseSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());
		}
	}
}

bool ADoor::IsOpen() const
{
	return Open;
}

/// Blueprint Events ///

void ADoor::OnOpen_Implementation()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}

void ADoor::OnClose_Implementation()
{
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
}
