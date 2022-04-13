// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/MenuWidget.h"
#include "TesserconCore.h"

#include "SelectEventMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API USelectEventMenuWidget : public UMenuWidget
{
	GENERATED_BODY()
public:
	USelectEventMenuWidget(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION()
		void GetEventInfo(const FString& EventID);
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:

	TSubclassOf<UUserWidget> SettingsWidgetClass;

	TArray<FTesserconEventsList> m_EventList;

	UFUNCTION()
		void GetEventInfoImage(UTexture2DDynamic* Texture);

	UFUNCTION()
		void GetEventOrganiserImage(UTexture2DDynamic* Texture);

	UFUNCTION()
		void GetEventSponsorImage(UTexture2DDynamic* Texture);

	UFUNCTION()
		void OpenOrganiserContactUrl();

	UFUNCTION()
		void OnSelectEventButtonClicked();

	UFUNCTION()
		void OnSettingsButtonClicked();

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* EventListScrollBox;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* EventSponsorsScrollBox;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* EventBannerSizeBox;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* EventInfoSizeBox;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* EventOrganiserSizeBox;

	UPROPERTY(meta = (BindWidget))
		class UButton* VisitOrganiserButton;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AttendeeCountText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* StartTimeText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* EndTimeText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* EventDescriptionText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* EventOrganiserText;

	UPROPERTY(meta = (BindWidget))
		class UButton* EventSelectButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* SettingsButton;

	FString m_OrganiserVisitUrl;

	void EventInfoCallBack(const ETesserconError& Error, const FTesserconEvent& EventInfo);
	
	void EventOrganiserCallBack(const ETesserconError& Error, const FTesserconEventOrganiser& EventOrganiser);

	void EventSponsorsCallBack(const ETesserconError& Error, const TArray<FString>& EventSponsorsUrl);
};