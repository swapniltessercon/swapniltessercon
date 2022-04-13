// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu/PlayerNamePlateWidget.h"

#include "UI/HUD/BaseHUD.h"
#include "UI/BaseMenuWidget.h"
#include "UI/Profile/UserProfileWidget.h"

#include "UI/TesserconButton.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPlayerNamePlateWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (UserProfileButton != nullptr)
	{
		UserProfileButton->OnClicked.AddDynamic(this, &UPlayerNamePlateWidget::ShowUserProfile);
	}
}

void UPlayerNamePlateWidget::NativeDestruct()
{
	//TODO
	Super::NativeDestruct();
}

void UPlayerNamePlateWidget::UpdateWidgetFields(const FString& InName, const FString& MatrixUsername, const FString& InUserDesignation, EUserRole InUserRole)
{
	if (InUserRole == EUserRole::Attendee)
	{
		AttendeeImage->SetVisibility(ESlateVisibility::Visible);
		ExhibitorImage->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		AttendeeImage->SetVisibility(ESlateVisibility::Hidden);
		ExhibitorImage->SetVisibility(ESlateVisibility::Visible);
	}

	if (PlayerNameText != nullptr)
	{
		PlayerNameText->SetText(FText::FromString(InName));
	}

	if (PlayerDesignationText != nullptr)
	{
		PlayerDesignationText->SetText(FText::FromString(InUserDesignation));
	}

	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		TesserconAPI::FOnUserInfoByUserIDResponseDelegate  RequestListCallback;
		RequestListCallback.BindLambda([this](ETesserconError Error, FTesserconUserInfo InUserInfo)
			{
				if (Error == ETesserconError::Success)
				{
					m_UserInfo = InUserInfo;
				}
			});
		TessClient->Getsession().GetUserInfoByUsername(MatrixUsername, RequestListCallback);
	}
}

void UPlayerNamePlateWidget::ShowUserProfile()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController != nullptr)
	{
		ABaseHUD* HUD = Cast<ABaseHUD>(PlayerController->GetHUD());

		if (HUD != nullptr)
		{
			UBaseMenuWidget* MenuWidget = Cast<UBaseMenuWidget>(HUD->GetCurrentMenuWidget());
			TSubclassOf<UUserWidget> UserProfileClass;
			MenuWidget->GetUserProfileClass(UserProfileClass);
			UUserProfileWidget* UserProfileWidget = CreateWidget<UUserProfileWidget>(GetWorld(), UserProfileClass);
			if (MenuWidget != nullptr && UserProfileWidget != nullptr)
			{
				FString UserID = GetMatrixFullUserIDForUsername(PlayerNameText->GetText().ToString());
				
				UserProfileWidget->SetProfile(m_UserInfo, UserID);
				MenuWidget->SetBlankSizeBox(UserProfileWidget);
			}
		}
	}
}

void UPlayerNamePlateWidget::IsSpeechDetected()
{
	/*TesserconSession* TessSession= &FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient()->Getsession();
	if (TessSession != nullptr)
	{
		IChannelSession* NearbyChannel = &TessSession->GetChannelSessionFromKey(EVivoxChannelKey::NearByChannel);
		if (NearbyChannel->IsTransmitting())
		{
			IParticipant* Participant = NearbyChannel->Participants()[m_UserInfo.Username];
			if (Participant->IsSelf())
			{
				if (Participant->SpeechDetected())
				{
					SpeakerImage->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					SpeakerImage->SetVisibility(ESlateVisibility::Hidden);
				}
			}
			else
			{
				SpeakerImage->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			SpeakerImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		SpeakerImage->SetVisibility(ESlateVisibility::Hidden);
	}*/
}
