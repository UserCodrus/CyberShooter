// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon.generated.h"

// A weapon that can be used by the player or an enemy
UCLASS(BlueprintType)
class CYBERSHOOTER_API UWeapon : public UDataAsset
{
	GENERATED_BODY()

public:
	// The projectile produced by the weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<AActor> Projectile;

	// The firing rate in beats
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float FireRate = 0.5;
	// A random angle added to bullets
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float FireAccuracy = 0.0f;

	// The number of bullets fired with each shot
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 NumBullets = 1;
	// The angle between bullets when multiple shots are fired
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float BulletOffset = 15.0f;
};
