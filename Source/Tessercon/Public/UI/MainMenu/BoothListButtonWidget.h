// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoothListButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UBoothListButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:

protected:
	void NativeConstruct();

	void NativeDestruct();
private:
	UPROPERTY(meta = (BindWidget))
		class UTesserconButton* BoothListButton;

	UPROPERTY(meta = (BindWidget))
		class UImage* BoothImage;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BoothNameText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BoothLocationText;

	UFUNCTION()
		void OnButtonClicked();

	UFUNCTION()
		void GetBoothImage(UTexture2DDynamic* Texture);

	FString m_BoothID;

	class UBoothListWidget* p_Parent;

	void SetInfo(class UBoothListWidget* parent, const FString& boothid, const FString& boothname, const FString& boothlocation);

	static UBoothListButtonWidget* p_SelectedButton;

	void SetupBoothListButton();

	friend class UBoothListWidget;
};
