// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "AggroInterface.h"

#include "CoreMinimal.h"
#include "Destructible.h"
#include "Spawner.generated.h"

class AEnemyMinion;

// A breakable enemy spawner
UCLASS()
class CYBERSHOOTER_API ASpawner : public ADestructible, public IAggroInterface
{
	GENERATED_BODY()
	
public:
	ASpawner();

	// Spawn a new child
	void SpawnChild();
	// Kill all the spawner's children
	void KillChildren();

	// Notify this spawner that one of its children has died
	void NotifyDeath(AEnemyMinion* Child);

	/// ICombatInterface ///

	virtual void Kill() override;

	/// IAggroInterface ///

	virtual void Aggro() override;
	virtual void EndAggro() override;
	virtual bool IsAggro() override;
	virtual void AggroReset() override;

	/// Accessors ///
	FORCEINLINE int32 GetAggro() { return AggroLevel; }

protected:
	// The type of enemy that spawns from this spawner
	UPROPERTY(Category = "Spawner", EditAnywhere)
		TSubclassOf<AEnemyMinion> EnemyType;
	// The minions that have been spawned by this spawner
	UPROPERTY(Category = "Spawner", VisibleAnywhere)
		TArray<AEnemyMinion*> Minions;
	// The number of minions that this spawner can spawn at once
	UPROPERTY(Category = "Spawner", EditAnywhere)
		int32 MaxMinions;
	// The time in seconds between spawns
	UPROPERTY(Category = "Spawner", EditAnywhere)
		float SpawnTime;
	// If set to true, all the minions of this spawner will die when the spawner dies
	UPROPERTY(Category = "Spawner", EditAnywhere)
		bool KillMinions;

	// The aggro counter for the enemy
	UPROPERTY(Category = "Aggro", EditAnywhere)
		int32 AggroLevel;
	// The aggro amount required to activate the enemy AI
	UPROPERTY(Category = "Aggro", EditAnywhere)
		int32 RequiredAggro;
	// The aggro required to deactive AI
	UPROPERTY(Category = "Aggro", EditAnywhere)
		int32 MinimumAggro;

	// The component that determines the spawn location of enemies
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly)
		USceneComponent* SpawnComponent;

	// The timer handle for enemy spawning
	FTimerHandle TimerHandle_SpawnTimer;
};
