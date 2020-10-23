// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class ACyberShooterPlayer;
class AEnemyBase;

// The base controller for enemy AI
UCLASS()
class CYBERSHOOTER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	virtual void BeginPlay() override;
	
	// Set to true when the player is close enough to activate the AI
	UPROPERTY(VisibleAnywhere)
		bool Aggro;

	// If set to true, the pawn will detect the player through walls
	UPROPERTY(Category = "Aim", VisibleAnywhere)
		bool SeeThroughWalls;
	// If set to true, the pawn will only fire perpendicular to its up vector, otherwise it will track the player in three dimensions
	UPROPERTY(Category = "Aim", VisibleAnywhere)
		bool ConstrainTargeting;
	// The speed at which the aiming vector rotates, in degrees per second
	UPROPERTY(Category = "Aim", VisibleAnywhere)
		float AimSpeed;
	// The maximum angle from which the pawn will shoot
	UPROPERTY(Category = "Aim", VisibleAnywhere)
		float FiringAngle;

	// The angle of movement relative to the player
	UPROPERTY(Category = "Movement", VisibleAnywhere)
		float MovementOffset;

protected:
	/// AI Functions ///
	
	// Determine whether or not the player is within the pawn's field of vision
	bool CheckLineOfSight();
	// Move the pawn's aiming component to track the player, returns the angle between the aim vector and the player
	float Aim(float DeltaTime);
	// Apply movement inputs to the pawn, returns the direction of movement
	FVector Move();

	// The player character
	UPROPERTY()
		ACyberShooterPlayer* Player;
	// The pawn being controlled by this AI
	UPROPERTY()
		AEnemyBase* EnemyPawn;
};