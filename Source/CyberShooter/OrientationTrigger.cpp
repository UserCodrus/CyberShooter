// Copyright © 2020 Brian Faubion. All rights reserved.

#include "OrientationTrigger.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterUtilities.h"

#include "Components/BoxComponent.h"

AOrientationTrigger::AOrientationTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create a collision box to generate overlap events
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("OrientationCollisionComponent"));
	Collision->SetCollisionProfileName("OverlapAll");
	Collision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AOrientationTrigger::BeginOverlap);
	RootComponent = Collision;

	OrientationVisualizer = CreateDefaultSubobject<UOrientationComponent>(TEXT("OrientationVisualizerComponent"));
	OrientationVisualizer->SetupAttachment(RootComponent);
	OrientationVisualizer->bIsEditorOnly = true;

	Forward = FVector(1.0f, 0.0f, 0.0f);
	Up = FVector(0.0f, 0.0f, 1.0f);
	RestrictOrientation = true;
	RequiredUp = FVector(0.0f, 0.0f, 1.0f);

	SetArrows();
}

void AOrientationTrigger::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SetArrows();
}

void AOrientationTrigger::PostLoad()
{
	Super::PostLoad();

	SetArrows();
}

void AOrientationTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Skip the orientation change if we are already in the proper orientation
		if (player->GetUpVector().GetSafeNormal().Equals(Up.GetSafeNormal()) && player->GetForwardVector().GetSafeNormal().Equals(Forward.GetSafeNormal()))
		{
			return;
		}

		// Make sure the player has the orientation required by the trigger
		if (RestrictOrientation)
		{
			if (!player->CheckOrientation(RequiredUp))
			{
				return;
			}
		}

		player->SetOrientation(Forward.GetSafeNormal(), Up.GetSafeNormal());
	}
}

void AOrientationTrigger::SetArrows()
{
	if (OrientationVisualizer != nullptr)
	{
		if (RestrictOrientation)
		{
			OrientationVisualizer->AlignArrows(Up, Forward, RequiredUp);
		}
		else
		{
			OrientationVisualizer->AlignArrows(Up, Forward, FVector(0.0f));
		}
	}
}