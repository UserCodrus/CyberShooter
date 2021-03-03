// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTrigger.generated.h"

// A trigger that sends the player to a different level
UCLASS()
class CYBERSHOOTER_API ALevelTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelTrigger();

protected:
	// Change levels when the player overlaps the trigger
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// The level the trigger will switch to
	UPROPERTY(EditAnywhere)
		FName TargetLevel;
	// The ID of this trigger
	UPROPERTY(EditAnywhere)
		uint32 TriggerID;
};
