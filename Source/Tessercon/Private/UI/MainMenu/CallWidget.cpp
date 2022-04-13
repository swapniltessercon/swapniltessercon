// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/CallWidget.h"
//
#include "UI/HUD/InGameHUD.h"
//Downloading
#include "Blueprint/AsyncTaskDownloadImage.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "UI/TesserconButton.h"



void UCallWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (EndCallButton != nullptr)
	{
		EndCallButton->OnClicked.AddDynamic(this, &UCallWidget::OnEndCallClicked);
	}

	if (MuteMicButton != nullptr)
	{
		MuteMicButton->OnClicked.AddDynamic(this, &UCallWidget::OnMuteMicClicked);
	}

	if (MuteSpeakerButton != nullptr)
	{
		MuteSpeakerButton->OnClicked.AddDynamic(this, &UCallWidget::OnMuteSpeakerClicked);
	}

	if (ChatButton != nullptr)
	{
		ChatButton->OnClicked.AddDynamic(this, &UCallWidget::OnChatClicked);
	}

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			
			TessClient->Getsession().d_EventOnCallStateChanged.AddUObject(this, &UCallWidget::OnResponseCallState);

		}
	}
}



void UCallWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().d_EventOnCallStateChanged.RemoveAll(this);
		}
	}
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}


void UCallWidget::NearBySetup()
{

	UserNameText->SetText(FText::FromString("NearBy"));
	DialingTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	CallTimeHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	EndCallButton->SetVisibility(ESlateVisibility::Collapsed);
	ChatButton->SetVisibility(ESlateVisibility::Collapsed);

	FString ImageUrlStr = FString::Printf(TEXT("/Game/BaseCore/Textures/CallTab/NearByIcon"));
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(ImageUrlStr)));
	if (UserProfileImage == nullptr) return;
	UserProfileImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	UserProfileImage->SetBrushFromTexture(Texture);

}

void UCallWidget::Setup(FTesserconCallState CallStateInfo)
{
	if (CallStateInfo.CallStatus == ETesserconCallStatus::Dialing)
	{
		DialingTextBlock->SetText(FText::FromString("Dialling ..."));
		CallTimeHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
		MuteSpeakerButton->SetVisibility(ESlateVisibility::Collapsed);
		MuteMicButton->SetVisibility(ESlateVisibility::Collapsed);
	}

	else if (CallStateInfo.CallStatus == ETesserconCallStatus::Connecting)
	{
		DialingTextBlock->SetText(FText::FromString("Connecting ..."));
		DialingTextBlock->SetVisibility(ESlateVisibility::Visible);
		CallTimeHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (CallStateInfo.CallStatus == ETesserconCallStatus::OnCall)
	{
		OnCallAction();
	}

	SetCallState(CallStateInfo);
}


void UCallWidget::OnCallAction()
{
	DialingTextBlock->SetText(FText::FromString("On Call ..."));
	CallTimeHorizontalBox->SetVisibility(ESlateVisibility::Visible);
	MuteSpeakerButton->SetVisibility(ESlateVisibility::Visible);
	MuteMicButton->SetVisibility(ESlateVisibility::Visible);
	EndCallButton->SetVisibility(ESlateVisibility::Visible);
	ChatButton->SetVisibility(ESlateVisibility::Visible);
	EndCallButton->SetIsEnabled(true);
	GetWorld()->GetTimerManager().SetTimer(UpdateCallTimer, this, &UCallWidget::UpdateCallDurationTimer, 1.0, true, 1.0);
}

FString UCallWidget::GetCallID()
{
	return CallStateInfoStored.CallID;
}

void UCallWidget::SetCallState(FTesserconCallState InCallStateInfo)
{
	CallStateInfoStored = InCallStateInfo;

	if (!bIsCallInfoTextSet)
	{
		SetCallInfo();
		bIsCallInfoTextSet = true;
	}
}

//Get Call Respons
void UCallWidget::OnResponseCallState(FTesserconCallState CallStateInfo)
{

	if (CallStateInfo.CallStatus == ETesserconCallStatus::Available || CallStateInfo.CallStatus == ETesserconCallStatus::Switching)
	{

		//TODO ::Check If Needed
		//RemoveFromParent();

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController != nullptr)
		{

			AInGameHUD* InGameHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
			if (InGameHUD != nullptr)
			{
				if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
				{
					//Set TarnsmissionMode as  Single
					TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
					TessClient->Getsession().SetNearByChannelTransmissionMode();
					NearBySetup();
				}
			}
			else
			{
				RemoveFromParent();
			}
		}
	}

	else if (CallStateInfo.CallStatus == ETesserconCallStatus::OnCall)
	{
		OnCallAction();
	}

	else if (CallStateInfo.CallStatus == ETesserconCallStatus::Processing)
	{
		EndCallButton->SetIsEnabled(false);
	}

	SetCallState(CallStateInfo);

}


//Set Call Info 
void UCallWidget::SetCallInfo()
{
	if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
	{
		m_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();

		if (m_ChatClient != nullptr)
		{
			if (CallStateInfoStored.CallType == ECallType::Team)
			{

				TSharedPtr<MatrixRoomSession> RoomSession = m_ChatClient->GetSession().GetRoomSession(CallStateInfoStored.RoomID);
				RoomName = RoomSession->GetRoomName();
				UserNameText->SetText(FText::FromString(RoomName));
				int Length = RoomName.Len() - 1;
				TeamGroupIconNameText->SetText(FText::FromString(RoomName.LeftChop(Length).ToUpper()));

				///TODO::
				/*FString ImageUrlStr = FString::Printf(TEXT("/Game/BaseCore/Textures/MainMenu/MainMenuFrontPage/MainMenugroupsLogo"));
				UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(ImageUrlStr)));
				if (UserProfileImage == nullptr) return;
				UserProfileImage->SetBrushFromTexture(Texture);*/

			}
			else if (CallStateInfoStored.CallType == ECallType::Individual)
			{
				FString UserID = m_ChatClient->GetSession().GetFriendUserIDForRoom(CallStateInfoStored.RoomID);
				FString  Username = GetUsernameFromMatrixUsername(UserID);
				TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
				if (TessClient != nullptr)
				{
					TesserconAPI::FOnUserInfoByUserIDResponseDelegate  RequestListCallback;
					RequestListCallback.BindLambda([this](ETesserconError Error, FTesserconUserInfo UserInfo)
						{
							if (Error == ETesserconError::Success)
							{
								FString Name = UserInfo.FirstName + " " + UserInfo.LastName;
								UserNameText->SetText(FText::FromString(Name));
								SetProfileImage(UserInfo.ProfileImageURL);

							}

						});
					TessClient->Getsession().GetUserInfoByUsername(Username, RequestListCallback);
				}

			}
		}
	}
}



void UCallWidget::SetProfileImage(FString ImageUrl)
{
	if (UserProfileImage != nullptr)
	{
		UAsyncTaskDownloadImage* DownloadedImageTask = NewObject<UAsyncTaskDownloadImage>();
		DownloadedImageTask->OnSuccess.AddDynamic(this, &UCallWidget::OnGetTexture);
		DownloadedImageTask->Start(ImageUrl);
	}
}
void UCallWidget::OnGetTexture(UTexture2DDynamic* Texture)
{
	if (UserProfileImage == nullptr) return;
	UserProfileImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	UserProfileImage->SetBrushFromTextureDynamic(Texture, true);
}




void UCallWidget::UpdateCallDurationTimer()
{
	if (Seconds != 59)
	{
		Seconds++;
		FText SecondsTime;
		SecondsTime = Conv_IntToFText(Seconds);
		SecondTextBlock->SetText(SecondsTime);
	}
	else
	{
		Seconds = 0;
		Minute++;
		FText MinuteTime;
		MinuteTime = Conv_IntToFText(Minute);
		MinuteTextBlock->SetText(MinuteTime);
	}
}

//Convert int to Text
FText UCallWidget::Conv_IntToFText(int32 Value, bool bAlwaysSign, bool bUseGrouping, int32 MinimumIntegralDigits, int32 MaximumIntegralDigits)
{
	FNumberFormattingOptions NumberFormatOptions;
	NumberFormatOptions.AlwaysSign = bAlwaysSign;
	NumberFormatOptions.UseGrouping = bUseGrouping;
	NumberFormatOptions.MinimumIntegralDigits = MinimumIntegralDigits;
	NumberFormatOptions.MaximumIntegralDigits = MaximumIntegralDigits;
	return FText::AsNumber(Value, &NumberFormatOptions);
}


//Call End
void UCallWidget::OnEndCallClicked()
{

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		if (TessClient != nullptr)
		{
			TessClient->Getsession().EndCall();

			//TODO :: Check its Needed or not [Not Needed]
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController != nullptr)
			{

				AInGameHUD* InGameHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
				if (InGameHUD != nullptr)
				{
					//Set TarnsmissionMode as  Single
					TessClient->Getsession().SetNearByChannelTransmissionMode();		
				}
			}
			//
		}
	}
}

void UCallWidget::OnChatClicked()
{
	//TODO
}


//Mute/unmute Speaker

void UCallWidget::OnMuteSpeakerClicked()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			if (bIsSpeakarEnable)
			{
				TessClient->Getsession().SetSpeakerMuted(bIsSpeakarEnable);
				MuteSpeakerButton->SetSelected();
				bIsSpeakarEnable = false;
			}
			else
			{
				TessClient->Getsession().SetSpeakerMuted(bIsSpeakarEnable);
				MuteSpeakerButton->SetNormal();
				bIsSpeakarEnable = true;
			}
		}
	}
}


//Mute/unmute Mic
void UCallWidget::OnMuteMicClicked()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		if (TessClient != nullptr)
		{

			if (bIsMicEnable)
			{
				TessClient->Getsession().SetMicMuted(bIsMicEnable);
				MuteMicButton->SetSelected();
				bIsMicEnable = false;
			}
			else
			{
				TessClient->Getsession().SetMicMuted(bIsMicEnable);
				MuteMicButton->SetNormal();
				bIsMicEnable = true;
			}
		}
	}
}