// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapBoothButton.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UMapBoothButton : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
	void NativeConstruct();

	void NativeDestruct();
private:
	FVector m_BoothMapLocation;

	FString m_BoothID;

	UUserWidget* p_Parent;

	UPROPERTY(meta = (BindWidget))
		class UButton* MapBoothButton;
	
	UPROPERTY(meta = (BindWidget))
		class UButton* TeleportButton;

	UPROPERTY(meta = (BindWidget))
		class UImage* BoothImage;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BoothNameText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BoothLocationText;

	UFUNCTION()
		void OnTeleportButtonClicked();

	UFUNCTION()
		void GetBoothImage(UTexture2DDynamic* Texture);


	void SetupBoothListButton();

	void SetInfo(UUserWidget* InParent, const FString& InBoothId, const FString& InBoothName, const FString& InBoothLocation, const FVector& InBoothMapLocation);

	friend class UMapMenuWidget;
};
