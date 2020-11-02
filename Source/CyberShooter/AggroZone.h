// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "LockInterface.h"
#include "AggroInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AggroZone.generated.h"

// A box that controls the aggro of enemies inside it
UCLASS()
class CYBERSHOOTER_API AAggroZone : public AActor, public ILockInterface
{
	GENERATED_BODY()
	
public:	
	AAggroZone();

	// Aggro enemies when the player enters the zone
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Remove aggro when the player exits the zone
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Update the list of enemies to include all enemies in this zone
	UFUNCTION(Category = "AI", CallInEditor)
		void UpdateEnemyList();

	void Respawn();

	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

protected:
	// If set to false, the zone will not activate when the player hits it
	UPROPERTY(Category = "AI", EditAnywhere)
		bool Active;
	// If set to true, enemies will respawn if they leave this zone and will respawn after being killed
	UPROPERTY(Category = "AI", EditAnywhere)
		bool RespawnEnemies;
	// The time it takes to respawn enemies after the player leaves the zone
	UPROPERTY(Category = "AI", EditAnywhere)
		float RespawnTime;
	// Actors controlled by this zone
	UPROPERTY(Category = "AI", EditInstanceOnly, meta = (MustImplement = "AggroInterface"))
		TArray<AActor*> Actors;

	// The collision box for handling overlaps
	UPROPERTY(Category = "Components", EditAnywhere)
		class UBoxComponent* CollisionBox;

	// The timer handle for enemy respawns
	FTimerHandle TimerHandle_RespawnTimer;
};
