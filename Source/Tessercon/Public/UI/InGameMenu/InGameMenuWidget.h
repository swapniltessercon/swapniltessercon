// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/BaseMenuWidget.h"

//#include "VivoxCore.h"
//#include "VivoxCoreCommon.h"
#include "IChannelSession.h"
#include "InGameMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UInGameMenuWidget : public UBaseMenuWidget
{
	GENERATED_BODY()
public:
	UInGameMenuWidget(const FObjectInitializer& ObjectInitializer);

	void OnVivoxChannelMessageRecived(const IChannelTextMessage& Message);

protected:
	void NativeConstruct() override;

	void NativeDestruct() override;
private:


	void SetNearByCall();
	UPROPERTY(meta = (BindWidget))
		class UTesserconButton* SidePanelNearbyButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* MiniMapButton;
	UPROPERTY(meta = (BindWidget))
		class UInGameMiniMapWidget* MiniMap;

	UFUNCTION()
		void OnNearbyChatButtonClicked();

	UFUNCTION()
		void OnMiniMapButtonClicked();

	UFUNCTION()
		void OnMessageContentCommitted(const FText& InText, ETextCommit::Type InCommitType);

	TSubclassOf<UUserWidget> MapMenuWidgetClass;
	TSubclassOf<UUserWidget> NeaybyWidgetClass;
	//TSubclassOf<UUserWidget> NeaybyCallWidgetClass;

	FTimerHandle LoadingMapTimer;
	void LoadChannelSession(ETesserconError Error);

	friend class AInGameHUD;
};
