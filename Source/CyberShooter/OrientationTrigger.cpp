// Copyright © 2020 Brian Faubion. All rights reserved.

#include "OrientationTrigger.h"
#include "CyberShooterPlayer.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

AOrientationTrigger::AOrientationTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &AOrientationTrigger::BeginOverlap);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	OrientationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Orientation"));
	OrientationComponent->SetUsingAbsoluteRotation(true);
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

	RequiredArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RequiredArrow"));
	RequiredArrow->SetupAttachment(RootComponent);
	RequiredArrow->SetRelativeLocation(FVector(0.0f, 25.0f, 0.0f));
	RequiredArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	RequiredArrow->ArrowColor = FColor::Yellow;
	RequiredArrow->bIsEditorOnly = true;
#endif

	RestrictOrientation = true;
}

void AOrientationTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Skip the orientation change if we are already in the proper orientation
		if (player->GetUpVector().GetSafeNormal().Equals(OrientationComponent->GetUpVector()) && player->GetForwardVector().GetSafeNormal().Equals(OrientationComponent->GetForwardVector()))
		{
			return;
		}

		// Make sure the player has the orientation required by the trigger
		if (RestrictOrientation)
		{
			if (!player->CheckOrientation(RootComponent->GetUpVector()))
			{
				return;
			}
		}

		player->SetOrientation(OrientationComponent->GetForwardVector(), OrientationComponent->GetUpVector());
	}
}