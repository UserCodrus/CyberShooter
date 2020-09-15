// Copyright © 2020 Brian Faubion. All rights reserved.

#include "VelocityTrigger.h"
#include "CyberShooterPlayer.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

/// AVelocityTrigger ///

AVelocityTrigger::AVelocityTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &AVelocityTrigger::BeginOverlap);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	DirectionComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Direction"));
	DirectionComponent->SetupAttachment(RootComponent);
	OrientationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Orientation"));
	OrientationComponent->SetupAttachment(RootComponent);

#if WITH_EDITOR
	DirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));
	DirectionArrow->SetupAttachment(DirectionComponent);
	DirectionArrow->ArrowColor = FColor::Turquoise;
	DirectionArrow->bIsEditorOnly = true;

	OrientationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("OrientationArrow"));
	OrientationArrow->SetupAttachment(OrientationComponent);
	OrientationArrow->SetRelativeLocation(FVector(0.0f, 25.0f, 0.0f));
	OrientationArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	OrientationArrow->ArrowColor = FColor::Yellow;
	OrientationArrow->bIsEditorOnly = true;
#endif

	// Set defaults
	Magnitude = 1000.0f;
	RestrictOrientation = true;
}

void AVelocityTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Make sure the player has the orientation required by the trigger
		if (RestrictOrientation)
		{
			if (!player->CheckOrientation(OrientationComponent->GetUpVector()))
			{
				return;
			}
		}
		
		// Apply the velocity boost in the direction the component is facing
		player->AddImpulse(DirectionComponent->GetForwardVector() * Magnitude);
	}
}