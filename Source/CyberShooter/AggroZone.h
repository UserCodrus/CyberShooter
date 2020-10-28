// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "LockInterface.h"

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

	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

protected:
	// If set to false, the zone will not activate when the player hits it
	UPROPERTY(Category = "AI", EditAnywhere)
		bool Active;
	// If set to true, enemies will respawn if they leave this zone - make sure enemies start inside the zone when the game begins
	UPROPERTY(Category = "AI", EditAnywhere)
		bool RespawnEnemies;
	// Enemies controlled by this zone
	UPROPERTY(Category = "AI", EditInstanceOnly)
		TArray<class AEnemyBase*> Enemies;

	// The collision box for handling overlaps
	UPROPERTY(Category = "Components", EditAnywhere)
		class UBoxComponent* CollisionBox;
};
