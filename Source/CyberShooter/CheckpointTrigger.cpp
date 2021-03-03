// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CheckpointTrigger.h"
#include "CyberShooterGameInstance.h"
#include "CyberShooterPlayer.h"

ACheckpointTrigger::ACheckpointTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &ACheckpointTrigger::BeginOverlap);
}

void ACheckpointTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Save the game and refill stats if the player overlaps the trigger
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		SaveGame();
		player->Refill();
	}
}

void ACheckpointTrigger::SaveGame()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		instance->SaveCheckpoint(GetActorLocation());
	}
}