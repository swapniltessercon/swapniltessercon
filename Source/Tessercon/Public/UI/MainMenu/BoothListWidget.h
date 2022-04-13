// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/MenuWidget.h"
#include "TesserconCore.h"

#include "BoothListWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UBoothListWidget : public UMenuWidget
{
	GENERATED_BODY()
public:

	UBoothListWidget(const FObjectInitializer& ObjectInitializer);

	void GetBoothInfo(const FString& boothid);

protected:

	void NativeConstruct();

	void NativeDestruct();

private:

	TArray<FTesserconBoothList> m_BoothList;

	TSubclassOf<UUserWidget> BoothListButtonWidgetClass;

	TSubclassOf<UUserWidget> ExhibitorListButtonWidgetClass;

	void PopulateBoothList();
	
	void PopulateExhibitorList(const TArray<FTesserconUserInfo>& ExhibitorList);

	UPROPERTY(meta = (BindWidget))
		class UButton* BoothListCloseButton;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* BoothListScrollBox;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* ExhibitorListScrollBox;

	UPROPERTY(meta = (BindWidget))
		class UImage* SelectedBoothBanner;

	UPROPERTY(meta = (BindWidget))
		class UImage* BoothLogoImage;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* SearchEditableTextBox;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BoothNameText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BoothLocationText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CompanyEmailText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CompanyPhoneText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BoothDescriptionText;

	UPROPERTY(meta = (BindWidget))
		class UImage* WidgetBackgroundImage;

	UFUNCTION()
		FEventReply OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent);

	UFUNCTION()
		void GetBoothInfoImage(UTexture2DDynamic* Texture);

	UFUNCTION()
		void OnSearchContentChanged(const FText& Text);

	void BoothInfoCallBack(const ETesserconError& Error, const FTesserconBooth& BoothInfo);
};
