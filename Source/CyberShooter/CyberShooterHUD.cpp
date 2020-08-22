// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterHUD.h"

#include "CyberShooterPlayer.h"

void ACyberShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(GetOwningPlayerController()->GetPawn());
	if (player != nullptr)
	{
		// Draw debug text
		if (DrawDebug)
		{
			int32 x, y;
			GetOwningPlayerController()->GetViewportSize(x, y);

			DrawText("HP", FLinearColor::White, x - 70, 20.0f);
			DrawText(FString::FromInt(player->GetHealth()), FLinearColor::White, x - 50, 20.0f);
			DrawText(FString::FromInt(player->GetMaxHealth()), FLinearColor::White, x - 25, 20.0f);

			DrawText("MO", FLinearColor::White, x - 70, 40.0f);
			DrawText(FString::FromInt(player->GetMomentum()), FLinearColor::White, x - 50, 40.0f);
			DrawText(FString::FromInt(player->GetMaxMomentum()), FLinearColor::White, x - 25, 40.0f);
		}
	}
}