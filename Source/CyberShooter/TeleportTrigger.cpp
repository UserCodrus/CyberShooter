// Copyright © 2020 Brian Faubion. All rights reserved.

#include "TeleportTrigger.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterUtilities.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

/// ATeleportBase ///

ATeleportBase::ATeleportBase()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->bVisualizeComponent = true;

	OrientationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Orientation"));
	OrientationComponent->SetupAttachment(RootComponent);

#if WITH_EDITOR
	OrientationUpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	OrientationUpArrow->SetupAttachment(OrientationComponent);
	OrientationUpArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	OrientationUpArrow->ArrowColor = FColor::Blue;
	OrientationUpArrow->bIsEditorOnly = true;

	OrientationForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
	OrientationForwardArrow->SetupAttachment(OrientationComponent);
	OrientationForwardArrow->ArrowColor = FColor::Red;
	OrientationForwardArrow->bIsEditorOnly = true;
#endif

	SetOrientation = true;
}

void ATeleportBase::OrientPlayer(ACyberShooterPlayer* Player)
{
	if (SetOrientation)
	{
		// Set the player's orientation to match the component
		Player->SetOrientation(OrientationComponent->GetForwardVector(), OrientationComponent->GetUpVector(), true);
	}
}

/// ATeleportTrigger ///

ATeleportTrigger::ATeleportTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &ATeleportTrigger::BeginOverlap);

	RequiredOrientationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RequiredOrientation"));
	RequiredOrientationComponent->SetupAttachment(RootComponent);

#if WITH_EDITOR
	RequiredArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RequiredArrow"));
	RequiredArrow->SetupAttachment(RequiredOrientationComponent);
	RequiredArrow->SetRelativeLocation(FVector(0.0f, 25.0f, 0.0f));
	RequiredArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	RequiredArrow->ArrowColor = FColor::Yellow;
	RequiredArrow->bIsEditorOnly = true;
#endif

	Target = nullptr;
	RestrictOrientation = true;
}

void ATeleportTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Target == nullptr)
		return;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Ensure that we didn't just get here from another telporter
		if (!player->CheckTeleport(this))
			return;

		// Make sure the player has the orientation required by the trigger
		if (RestrictOrientation)
		{
			if (!player->CheckOrientation(RequiredOrientationComponent->GetUpVector()))
			{
				return;
			}
		}

		// Teleport the player
		player->Teleport(this, Target);
	}
}