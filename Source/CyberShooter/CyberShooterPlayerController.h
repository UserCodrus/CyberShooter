// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CyberShooterPlayerController.generated.h"

class UWeapon;

// The main player controller
UCLASS()
class CYBERSHOOTER_API ACyberShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACyberShooterPlayerController();

	/// Menus ///

	// Open the pause menu
	void OpenPauseMenu();
	// Open the weapon select wheel
	void OpenWeaponSelect();
	// Close the weapon select wheel
	void CloseWeaponSelect();
	// Close all open menus
	void CloseMenus();

	// Returns true if a radial menu is open
	bool IsMenuOpen();

	/// Player Interface ///

	// Get the number of segments required for the weapon radial menu
	UFUNCTION(BlueprintCallable)
		int32 GetWeaponSlots();
	// Get the player's weapon set
	UFUNCTION(BlueprintCallable)
		TArray<UWeapon*> GetPlayerWeapons();

	// Get the angle of the current radial selection
	UFUNCTION(BlueprintCallable)
		float GetRadialAngle();
	// Get the aiming direction
	UFUNCTION(BlueprintCallable)
		FVector GetFireDirection();

protected:
	/// APlayerController ///

	virtual void SetupInputComponent() override;

	/// Player Management ///
	// Force the player to select a weapon based on radial menu inputs
	void SelectWeapon();

	// The game speed while a radial menu is open
	UPROPERTY(Category = Gameplay, EditAnywhere)
		float MenuSpeed;

	// Bindings for the radial menu
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

	// Set to true when a menu is opened
	bool MenuOpen;
};
