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

	virtual void BeginPlay() override;

	// Aggro enemies when the player enters the zone
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Remove aggro when the player exits the zone
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Update the list of enemies to include all enemies in this zone
	UFUNCTION(Category = "AI", CallInEditor)
		void UpdateActorList();

	// Notify the zone that an enemy has been registered
	void NotifyRegister();
	// Notify the zone that one of its actors has despawned
	void NotifyDespawn();

	// Force the zone to respawn
	void Respawn();
	// Disable the aggro zone
	void Disable();

	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

protected:
	// If set to false, the zone will not activate when the player hits it
	UPROPERTY(Category = "AI", EditAnywhere)
		bool Active;
	// If set to true, the player will only trigger the zone if its orientation is the same as the zone
	UPROPERTY(Category = "AI", EditAnywhere)
		bool RestrictOrientation;
	// If set to true, enemies will respawn if they leave this zone and will respawn after being killed
	UPROPERTY(Category = "AI", EditAnywhere)
		bool RespawnActors;
	// The time it takes to respawn enemies after the player leaves the zone
	UPROPERTY(Category = "AI", EditAnywhere)
		float RespawnTime;
	// Actors controlled by this zone
	UPROPERTY(Category = "AI", EditInstanceOnly, meta = (MustImplement = "AggroInterface"))
		TArray<AActor*> Actors;

	// If set to true, the zone will reset when unlocked
	UPROPERTY(Category = "Lock", EditAnywhere)
		bool ResetOnUnlock;
	// Disable the enemies in the zone when the zone is inactive
	UPROPERTY(Category = "Lock", EditAnywhere)
		bool DisableOnDeactivate;

	// The lock that this zone unlocks when its enemies are dead
	UPROPERTY(Category = "Lock|Clear", EditInstanceOnly, meta = (MustImplement = "LockInterface"))
		AActor* ClearTarget;
	// Reverses the locking system to lock when it would normally unlock
	UPROPERTY(Category = "Lock|Clear", EditAnywhere)
		bool ClearInvert;
	// If set to true, the zone will only unlock the first time it is cleared
	UPROPERTY(Category = "Lock|Clear", EditAnywhere)
		bool ClearPermanentUnlock;

	// The lock that this zone unlocks when the player enters it
	UPROPERTY(Category = "Lock|Overlap", EditInstanceOnly, meta = (MustImplement = "LockInterface"))
		AActor* OverlapTarget;
	// Reverses the locking system to lock when it would normally unlock
	UPROPERTY(Category = "Lock|Overlap", EditAnywhere)
		bool OverlapInvert;
	// If set to true, the zone will only unlock when the player enters it for the first time
	UPROPERTY(Category = "Lock|Overlap", EditAnywhere)
		bool OverlapPermanentUnlock;

	// The collision box for handling overlaps
	UPROPERTY(Category = "Components", EditAnywhere)
		class UBoxComponent* CollisionBox;

	// The sound played when the zone is cleared
	UPROPERTY(Category = "Sound", EditAnywhere)
		USoundBase* ClearSound;

	// The number of enemies that this zone is tracking
	UPROPERTY(EditAnywhere)
	int32 TotalEnemies;
	// The number of tracked enemies that have despawned
	UPROPERTY(EditAnywhere)
	int32 DespawnedEnemies;
	// The timer handle for enemy respawns
	FTimerHandle TimerHandle_RespawnTimer;
	// Set to true when the player has activated the aggro zone
	bool Aggro;
	// Set to true after the zone is cleared for the first time
	bool Cleared;
	// Set to true when the overlap lock is triggered
	bool Unlocked;

#if WITH_EDITORONLY_DATA
	// Arrow indicating the orientation of the zone
	UPROPERTY(Category = "Components", EditAnywhere)
		class UArrowComponent* OrientationArrow;
#endif
};
