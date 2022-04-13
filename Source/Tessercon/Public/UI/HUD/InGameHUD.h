// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUD/BaseHUD.h"
#include "InGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API AInGameHUD : public ABaseHUD
{
	GENERATED_BODY()
public:
	AInGameHUD();

	float m_MapZoom;
	float m_MapDimension;
	UTexture2D* m_MapImage;

	UFUNCTION(Blueprintcallable)
		void SetMap(float InZoom, float InDimensions, UTexture2D* InImage);
protected:
	virtual void BeginPlay() override;
};
