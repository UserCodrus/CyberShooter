// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "PhysicsInterface.h"

#include "CoreMinimal.h"
#include "CyberShooterPawn.h"
#include "CyberShooterEnemy.generated.h"

// A standard enemy type
UCLASS(Abstract, BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyBase : public ACyberShooterPawn
{
	GENERATED_BODY()
	
public:
	AEnemyBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// The function that handles the enemy hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/// ICombatInterface ///

	virtual void Kill() override;

	/// Accessors ///
	
	FORCEINLINE int32 GetAggro() { return AggroLevel; }
	FORCEINLINE bool CanSeeThroughWalls() { return SeeThroughWalls; }

	// Get the direction the pawn is aiming
	FVector GetAimVector() const;
	// Get the rotation of the pawn's aim component in world space
	FRotator GetAimRotation() const;
	// Get the rotation of the pawn's core component in local space
	FRotator GetAimLocalRotation() const;

	// Set the rotation of the aim component in wolrd space
	void SetAimRotation(FRotator Rotation);
	void SetAimRotation(FQuat Rotation);
	// Set the rotation of the aim component in wolrd space
	void SetAimWorldRotation(FRotator Rotation);
	void SetAimWorldRotation(FQuat Rotation);

	// Increase the aggro counter for the enemy
	void Aggro();
	// Decrease the aggro counter for the enemy
	void DeAggro();
	// Check to see if the enemy is attacking the player
	bool IsAggro();

	// Order the pawn to move in a direction
	virtual void AddControlInput(FVector WorldVector) {};

protected:
	/// AI ///

	// The aggro counter for the enemy
	UPROPERTY(Category = "AI", EditAnywhere)
		int32 AggroLevel;
	// The aggro amount required to activate the enemy AI
	UPROPERTY(Category = "AI", EditAnywhere)
		int32 RequiredAggro;
	// The aggro required to deactive AI
	UPROPERTY(Category = "AI", EditAnywhere)
		int32 MinimumAggro;

	// If set to true, the enemy will track the player through walls
	UPROPERTY(Category = "AI", EditAnywhere)
		bool SeeThroughWalls;

	/// Combat ///

	// Damage the enemy deals on contact to players
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, BlueprintReadWrite)
		int32 ContactDamage;

	// The weapon that will be fired when the pawn dies
	UPROPERTY(Category = "Weapon", EditAnywhere, BlueprintReadWrite)
		UWeapon* DeathWeapon;

	/// Components ///
	// The component that controls the pawn's weapon aiming
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly)
		USceneComponent* AimComponent;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyTurret : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyTurret();

	virtual void BeginPlay() override;

	/// Accessors ///

	FORCEINLINE bool IsTargetingConstrained() { return ConstrainTargeting; }
	FORCEINLINE float GetRotationSpeed() { return RotationSpeed; }
	FORCEINLINE float GetFiringAngle() { return MinimumFiringAngle; }

protected:
	/// AI ///
	
	// If set to true, the turret will only fire orthagonal to its position, otherwise it will target the player in three dimensions
	UPROPERTY(Category = "AI", EditAnywhere)
		bool ConstrainTargeting;
	// The rate at which the turret can turn in degrees per second
	UPROPERTY(Category = "AI", EditAnywhere)
		float RotationSpeed;
	// The minimum angle between the turret and the player before the turret will fire
	UPROPERTY(Category = "AI", EditAnywhere)
		float MinimumFiringAngle;
	// If set to true the turret will attempt to track the player's movement when aiming
	UPROPERTY(Category = "AI", EditAnywhere)
		bool TrackPlayer;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyMovingTurret : public AEnemyTurret
{
	GENERATED_BODY()

public:
	AEnemyMovingTurret();

	virtual FVector GetVelocity() const override;

	/// Spline Utilities ///

	// Move the enemy to the start point of the spline
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void MoveEnemyToStart();
	// Move the enemy to the end point of the spline
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void MoveEnemyToEnd();
	// Erase all the spline's points and place it at the center of the enemy
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void ResetSpline();
	// Move the spline's start to the center of the enemy
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void MoveSpline();
	// Add a new spline point at the enemies current position
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void AppendSpline();

protected:
	/// Properties ///
	
	// The spline component that dictates the turret's patrol path
	UPROPERTY(Category = "Components", EditAnywhere)
		class USplineComponent* Spline;
	// The movement component for the turret
	UPROPERTY(Category = "Components", EditAnywhere)
		class USplineMovementComponent* MovementComponent;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemySeeker : public AEnemyBase, public IPhysicsInterface
{
	GENERATED_BODY()

public:
	AEnemySeeker();

	virtual void BeginPlay() override;

	// Handle the seeker hitting physics objects
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Order the pawn to move in a direction
	virtual void AddControlInput(FVector WorldVector);

	/// IPhysicsInterface ///

	FVector GetVelocity() override;
	void ChangeVelocity(FVector NewVelocity) override;
	void AddImpulse(FVector Force) override;
	void AddRelativeImpulse(FVector Force) override;
	void AddStaticForce(FVector Force) override;
	void RemoveStaticForce(FVector Force) override;
	void ResetStaticForce() override;
	void SetGravity(float NewGravity) override;
	void ResetGravity() override;
	void SetAirFriction(float NewFriction) override;
	void ResetAirFriction() override;
	void SetFriction(float NewFriction) override;
	void ResetFriction() override;
	void SetMass(float Multiplier) override;
	void ResetMass() override;
	void SetTickRate(float NewRate) override;
	float GetMass() const override;
	float GetWeight() const override;

protected:
	/// Properties ///

	// The gravity modification from abilities
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float GravityMultiplier;
	// The acceleration of gravity in the world
	UPROPERTY(Category = "Movement|Physics", VisibleAnywhere)
		float WorldGravity;
	// The mass of the pawn
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float Mass;
	
	// The movement component for the seeker
	UPROPERTY(Category = "Components", EditAnywhere)
		class UEnemyMovementComponent* MovementComponent;
};