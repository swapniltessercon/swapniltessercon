// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Settings/SettingsMenuWidget.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/InGameHUD.h"

#include "Components/SizeBox.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/ProgressBar.h"
#include "Components/CheckBox.h"

#include "VivoxCore.h"

static IAudioDevices* InputDevices;
static IAudioDevices* OutputDevices;
static UGameUserSettings* GraphicsSettings;


void USettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GraphicsSettings = GEngine->GameUserSettings;

	this->CurrentGraphics();

	if (SpeakerVolumeSlider != nullptr)
	{
		SpeakerVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnVolumeChange);
	}

	if (SettingsCloseButton != nullptr)
	{
		SettingsCloseButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::RemoveFromParent);
	}
	
	if (BlurImage != nullptr)
	{
		BlurImage->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDownImage"));
	}

	if (MicrophoneComboBox != nullptr)
	{
		PopulateMicrophoneComboBox();
		MicrophoneComboBox->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnMicrophoneChange);
	}

	if (MuteCheckBox != nullptr)
	{
		MuteCheckBox->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget::OnMuteStatusChange);
	}

	if (SpeakerComboBox != nullptr)
	{
		PopulateSpeakerComboBox();
		SpeakerComboBox->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnSpeakerChange);
	}

	if (VideoQualityNextButton != nullptr)
	{
		VideoQualityNextButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityNextButtonClicked);
	}

	if (VideoQualityPreviousButton != nullptr)
	{
		VideoQualityPreviousButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityPreviousButtonClicked);
	}

	if (LogoutMenuButton != nullptr)
	{
		LogoutMenuButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnLogoutMenuButtonClicked);
	}

	if (LogoutButton != nullptr)
	{
		LogoutButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnLogoutButtonClicked);
	}

	if (LogoutCancelButton != nullptr)
	{
		LogoutCancelButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::RemoveFromParent);
	}

	if (Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) != nullptr && QuitMainMenuButton != nullptr)
	{
		QuitMainMenuButton->SetVisibility(ESlateVisibility::Visible);
	}

	if (QuitTesserconButton != nullptr)
	{
		QuitTesserconButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnQuitTesserconButtonClicked);
	}

	if (QuitButton != nullptr)
	{
		QuitButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnQuitButtonClicked);
	}

	if (QuitCancelButton != nullptr)
	{
		QuitCancelButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::RemoveFromParent);
	}

	if (QuitMainMenuButton != nullptr)
	{
		QuitMainMenuButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnQuitMainMenuButtonClicked);
	}

	if (QuittoMainMenuButton != nullptr)
	{
		QuittoMainMenuButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnQuittoMainMenuButtonClicked);
	}

	if (QuittoMainMenuCancelButton != nullptr)
	{
		QuittoMainMenuCancelButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::RemoveFromParent);
	}
}

void USettingsMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

FEventReply USettingsMenuWidget::OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent)
{
	this->RemoveFromParent();
	return FEventReply(false);
}

void USettingsMenuWidget::PopulateMicrophoneComboBox()
{
	FString ActiveMic = "";
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			InputDevices = TessClient->Getsession().GetInputAudioDevices();
			ActiveMic = TessClient->Getsession().GetActiveMic()->Name();
		}
	}

	for (auto Pair : InputDevices->AvailableDevices())
	{
		MicrophoneComboBox->AddOption(Pair.Value->Name());
	}
	MicrophoneComboBox->SetSelectedOption(ActiveMic);

	this->CurrentVolume();
}

void USettingsMenuWidget::PopulateSpeakerComboBox()
{
	FString ActiveSpeaker = "";
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			OutputDevices = TessClient->Getsession().GetOutputAudioDevices();
			ActiveSpeaker = TessClient->Getsession().GetActiveSpeaker()->Name();
		}
	}
	for (auto Pair : OutputDevices->AvailableDevices())
	{
		SpeakerComboBox->AddOption(Pair.Value->Name());
	}
	SpeakerComboBox->SetSelectedOption(ActiveSpeaker);
}

void USettingsMenuWidget::SetVideoGraphics(const FString& Quality)
{
	if (Quality == "Low")
	{
		GraphicsSettings->ScalabilityQuality.SetViewDistanceQuality(2);
		GraphicsSettings->ScalabilityQuality.SetAntiAliasingQuality(2);
		GraphicsSettings->ScalabilityQuality.SetShadowQuality(2);
		GraphicsSettings->ScalabilityQuality.SetEffectsQuality(2);
		GraphicsSettings->ScalabilityQuality.SetTextureQuality(2);
		GraphicsSettings->ScalabilityQuality.SetPostProcessQuality(2);
		GraphicsSettings->ScalabilityQuality.SetFoliageQuality(2);
		GraphicsSettings->ScalabilityQuality.SetShadingQuality(2);
		GraphicsSettings->SetResolutionScaleValueEx(40);
		GraphicsSettings->ValidateSettings();
		GraphicsSettings->ApplySettings(true);
	}
	if (Quality == "Medium")
	{
		GraphicsSettings->ScalabilityQuality.SetViewDistanceQuality(3);
		GraphicsSettings->ScalabilityQuality.SetAntiAliasingQuality(3);
		GraphicsSettings->ScalabilityQuality.SetShadowQuality(3);
		GraphicsSettings->ScalabilityQuality.SetEffectsQuality(3);
		GraphicsSettings->ScalabilityQuality.SetTextureQuality(3);
		GraphicsSettings->ScalabilityQuality.SetPostProcessQuality(3);
		GraphicsSettings->ScalabilityQuality.SetFoliageQuality(3);
		GraphicsSettings->ScalabilityQuality.SetShadingQuality(3);
		GraphicsSettings->SetResolutionScaleValueEx(60);
		GraphicsSettings->ValidateSettings();
		GraphicsSettings->ApplySettings(true);
	}
	if (Quality == "High")
	{
		GraphicsSettings->ScalabilityQuality.SetViewDistanceQuality(4);
		GraphicsSettings->ScalabilityQuality.SetAntiAliasingQuality(4);
		GraphicsSettings->ScalabilityQuality.SetShadowQuality(4);
		GraphicsSettings->ScalabilityQuality.SetEffectsQuality(4);
		GraphicsSettings->ScalabilityQuality.SetTextureQuality(4);
		GraphicsSettings->ScalabilityQuality.SetPostProcessQuality(4);
		GraphicsSettings->ScalabilityQuality.SetFoliageQuality(4);
		GraphicsSettings->ScalabilityQuality.SetShadingQuality(4);
		GraphicsSettings->SetResolutionScaleValueEx(100);
		GraphicsSettings->ValidateSettings();
		GraphicsSettings->ApplySettings(true);
	}
}

void USettingsMenuWidget::CurrentGraphics()
{
	int Value = GraphicsSettings->GetAntiAliasingQuality();

	if (Value == 4)
	{
		VideoQualityText->SetText(FText::FromString("High"));
		VideoQualityNextButton->SetIsEnabled(false);
		VideoQualityPreviousButton->SetIsEnabled(true);
	}
	else if (Value == 3)
	{
		VideoQualityText->SetText(FText::FromString("Medium"));
		VideoQualityNextButton->SetIsEnabled(true);
		VideoQualityPreviousButton->SetIsEnabled(true);
	}
	else
	{
		VideoQualityText->SetText(FText::FromString("Low"));
		VideoQualityNextButton->SetIsEnabled(true);
		VideoQualityPreviousButton->SetIsEnabled(false);
	}
}

void USettingsMenuWidget::CurrentVolume()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			float Volume = TessClient->Getsession().GetSpeakerVolume();

			if (!TessClient->Getsession().GetInputAudioDevices()->Muted())
			{
				MuteCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
			}
			else
			{
				MuteCheckBox->SetCheckedState(ECheckBoxState::Checked);
			}
			SpeakerVolumeSlider->SetValue(Volume);
			SpeakerVolumeProgressBar->SetPercent(Volume);
		}
	}
}

void USettingsMenuWidget::OnMicrophoneChange(FString SelectedOption, ESelectInfo::Type SelectType)
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().SetActiveMic(SelectedOption);
		}
	}
}

void USettingsMenuWidget::OnMuteStatusChange(bool IsChecked)
{
	InputDevices->SetMuted(IsChecked);
}

void USettingsMenuWidget::OnSpeakerChange(FString SelectedOption, ESelectInfo::Type SelectType)
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().SetActiveSpeaker(SelectedOption);
		}
	}
}

void USettingsMenuWidget::OnVolumeChange(float Volume)
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().SetSpeakerVolume(Volume);
			SpeakerVolumeProgressBar->SetPercent(Volume);
		}
	}
}

void USettingsMenuWidget::OnVideoQualityNextButtonClicked()
{
	FString VideoQuality;
	if (VideoQualityText != nullptr)
	{
		VideoQuality = VideoQualityText->GetText().ToString();
		if (VideoQuality == "Low")
		{
			VideoQualityText->SetText(FText::FromString("Medium"));
			SetVideoGraphics("Medium");
			VideoQualityPreviousButton->SetIsEnabled(true);
		}
		if (VideoQuality == "Medium")
		{
			VideoQualityText->SetText(FText::FromString("High"));
			SetVideoGraphics("High");
			VideoQualityNextButton->SetIsEnabled(false);
		}
	}
}

void USettingsMenuWidget::OnVideoQualityPreviousButtonClicked()
{
	FString VideoQuality;
	if (VideoQualityText != nullptr)
	{
		VideoQuality = VideoQualityText->GetText().ToString();
		if (VideoQuality == "High")
		{
			VideoQualityText->SetText(FText::FromString("Medium"));
			SetVideoGraphics("Medium");
			VideoQualityNextButton->SetIsEnabled(true);
		}
		if (VideoQuality == "Medium")
		{
			VideoQualityText->SetText(FText::FromString("Low"));
			SetVideoGraphics("Low");
			VideoQualityPreviousButton->SetIsEnabled(false);
		}
	}
}

void USettingsMenuWidget::OnLogoutMenuButtonClicked()
{
	SettingsMenuSizeBox->SetVisibility(ESlateVisibility::Hidden);
	LogoutMenuSizeBox->SetVisibility(ESlateVisibility::Visible);
}

void USettingsMenuWidget::OnQuitTesserconButtonClicked()
{
	SettingsMenuSizeBox->SetVisibility(ESlateVisibility::Hidden);
	QuitGameMenuSizeBox->SetVisibility(ESlateVisibility::Visible);
}

void USettingsMenuWidget::OnQuitMainMenuButtonClicked()
{
	SettingsMenuSizeBox->SetVisibility(ESlateVisibility::Hidden);
	QuitMainMenuSizeBox->SetVisibility(ESlateVisibility::Visible);
}

void USettingsMenuWidget::OnLogoutButtonClicked()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().Logout();
		}
	}
}

void USettingsMenuWidget::OnQuittoMainMenuButtonClicked()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FString TravelURL = "/Game/BaseCore/Maps/MainMenu/MainMenuLobby";

	PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
}

void USettingsMenuWidget::OnQuitButtonClicked()
{
	FGenericPlatformMisc::RequestExit(false);
}