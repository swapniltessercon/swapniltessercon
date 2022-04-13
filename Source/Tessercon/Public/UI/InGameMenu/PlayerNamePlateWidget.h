// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "TesserconCoreCommon.h"

#include "PlayerNamePlateWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UPlayerNamePlateWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void UpdateWidgetFields(const FString& InName, const FString& MatrixUsername, const FString& InUserDesignation, EUserRole InUserRole);
	void IsSpeechDetected();

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	FTesserconUserInfo m_UserInfo;

	UPROPERTY(meta = (BindWidget))
		class UImage* AttendeeImage;
	UPROPERTY(meta = (BindWidget))
		class UImage* ExhibitorImage;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PlayerNameText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PlayerDesignationText;
	UPROPERTY(meta = (BindWidget))
		class UImage* SpeakerImage;
	UPROPERTY(meta = (BindWidget))
		class UButton* UserProfileButton;

	UFUNCTION()
		void ShowUserProfile();
};
