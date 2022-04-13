// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MenuWidget.h"

#include "TesserconCore.h"

#include "SettingsMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API USettingsMenuWidget : public UMenuWidget
{
	GENERATED_BODY()

public:

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:

	void PopulateMicrophoneComboBox();

	void PopulateSpeakerComboBox();

	void SetVideoGraphics(const FString& Quality);

	void CurrentGraphics();

	void CurrentVolume();

	UPROPERTY(meta = (BindWidget))
		class UImage* BlurImage;

	UPROPERTY(meta = (BindWidget))
		class UButton* SettingsCloseButton;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* SettingsMenuSizeBox;

	UPROPERTY(meta = (BindWidget))
		class UButton* AudioVideoButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* LogoutMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitMainMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitTesserconButton;

	UPROPERTY(meta = (BindWidget))
		class UComboBoxString* MicrophoneComboBox;

	UPROPERTY(meta = (BindWidget))
		class UCheckBox* MuteCheckBox;

	UPROPERTY(meta = (BindWidget))
		class UComboBoxString* SpeakerComboBox;

	UPROPERTY(meta = (BindWidget))
		class USlider* SpeakerVolumeSlider;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* SpeakerVolumeProgressBar;

	UPROPERTY(meta = (BindWidget))
		class UButton* VideoQualityPreviousButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* VideoQualityNextButton;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* VideoQualityText;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* LogoutMenuSizeBox;

	UPROPERTY(meta = (BindWidget))
		class UButton* LogoutButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* LogoutCancelButton;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* QuitGameMenuSizeBox;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitCancelButton;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* QuitMainMenuSizeBox;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuittoMainMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuittoMainMenuCancelButton;

	UFUNCTION()
		FEventReply OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent);

	UFUNCTION()
		void OnMicrophoneChange(FString SelectedOption, ESelectInfo::Type SelectType);

	UFUNCTION()
		void OnMuteStatusChange(bool IsChecked);

	UFUNCTION()
		void OnSpeakerChange(FString SelectedOption, ESelectInfo::Type SelectType);

	UFUNCTION()
		void OnVolumeChange(float Volume);

	UFUNCTION()
		void OnVideoQualityNextButtonClicked();

	UFUNCTION()
		void OnVideoQualityPreviousButtonClicked();

	UFUNCTION()
		void OnLogoutMenuButtonClicked();

	UFUNCTION()
		void OnQuitTesserconButtonClicked();

	UFUNCTION()
		void OnQuitMainMenuButtonClicked();

	UFUNCTION()
		void OnLogoutButtonClicked();

	UFUNCTION()
		void OnQuittoMainMenuButtonClicked();

	UFUNCTION()
		void OnQuitButtonClicked();
};
