// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMiniMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UInGameMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float m_Zoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float m_Dimension;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UTexture2D* p_MiniMapImage;

protected:
	virtual void NativeConstruct() override;
private:
	UFUNCTION(Blueprintcallable, meta = (AllowPrivateAccess = "true"))
		void SetupMap();
};
