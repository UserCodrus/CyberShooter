// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CyberShooterPawn.h"
#include "CyberShooterEnemy.generated.h"

// A standard enemy type
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ACyberShooterEnemy : public ACyberShooterPawn
{
	GENERATED_BODY()
	
public:
	ACyberShooterEnemy();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void Kill();

	// The function that handles the enemy hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/// Accessors ///
	FORCEINLINE float GetAggroDistance() { return AggroDistance; }

protected:
	/// AI ///

	// The distance from which the enemy will begin to attack the player
	UPROPERTY(Category = "AI", EditAnywhere)
		float AggroDistance;

	/// Combat ///

	// Damage the enemy deals on contact to players
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, BlueprintReadWrite)
		int32 ContactDamage;

	// The weapon that will be fired when the pawn dies
	UPROPERTY(Category = "Weapon", EditAnywhere, BlueprintReadWrite)
		UWeapon* DeathWeapon;
};
