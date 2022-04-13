// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/CallerNotifyProfileWidget.h"

//
#include "UI/HUD/InGameHUD.h"
//Downloading
#include "Blueprint/AsyncTaskDownloadImage.h"
#include "UI/MainMenu/CallInfoListWidget.h"
//Components
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


void UCallerNotifyProfileWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RejectCallButton != nullptr)
	{
		RejectCallButton->OnClicked.AddDynamic(this, &UCallerNotifyProfileWidget::OnRejectCallButtonClicked);
	}
	if (AcceptCallButton != nullptr)
	{
		AcceptCallButton->OnClicked.AddDynamic(this, &UCallerNotifyProfileWidget::OnAcceptCallButtonClicked);

	}
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().d_EventOnMatrixCallEventProcessed.AddUObject(this, &UCallerNotifyProfileWidget::OnResponseCallEvent);
		}
	}

}
void UCallerNotifyProfileWidget::OnRejectCallButtonClicked()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			//TODO::
			TesserconSession::FOnTesserconSessionResponseDelegate CallHangupCallBack;
			CallHangupCallBack.BindLambda([this](ETesserconError Error)
				{
					if (Error == ETesserconError::Success)
					{
						RemoveFromParent();
					}
				});
			TessClient->Getsession().HangupCallInvite(RoomID, Invite.CallID, CallHangupCallBack);
		}
	}



}
void UCallerNotifyProfileWidget::OnAcceptCallButtonClicked()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		if (TessClient != nullptr)
		{
			TessClient->Getsession().AnswerCall(RoomID, Invite);
		
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController != nullptr)
			{
				ABaseHUD* HUD = Cast<ABaseHUD>(PlayerController->GetHUD());
				if (HUD != nullptr)
				{
					UBaseMenuWidget* MenuWidget = Cast<UBaseMenuWidget>(HUD->GetCurrentMenuWidget());
					if (MenuWidget != nullptr)
					{
						//Hide Call Notification Widget
						MenuWidget->HideCallNotification();
						AInGameHUD* InGameHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
						if (InGameHUD != nullptr)
						{
							////Set TarnsmissionMode as  None
							TessClient->Getsession().UnSetNearByChannelTransmissionMode();
						}
						RemoveFromParent();
					}
					
				}
			}
			
		}
	}
}

void UCallerNotifyProfileWidget::InitalSetup(class UCallInfoListWidget* InParent, FMatrixCallEvent InviteInfo, FString InRoomID)
{
	RoomID = InRoomID;
	Invite = InviteInfo;
	Parent = InParent;

	FString Username = GetUsernameFromMatrixUsername(Invite.Sender);
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		TesserconAPI::FOnUserInfoByUserIDResponseDelegate RequestListCallback;
		RequestListCallback.BindLambda([this, InviteInfo](ETesserconError Error, FTesserconUserInfo UserInfo)
			{
				if (Error == ETesserconError::Success)
				{
					SetProfileInfo(UserInfo);
				}
			});
		TessClient->Getsession().GetUserInfoByUsername(Username, RequestListCallback);
	}

}


void UCallerNotifyProfileWidget::SetProfileInfo(FTesserconUserInfo InUserInfo)
{

	if (UserNameTextBlock != nullptr && DesignationTextBlock != nullptr)
	{
		FString Name = InUserInfo.FirstName + " " + InUserInfo.LastName;
		UserNameTextBlock->SetText(FText::FromString(Name));
		DesignationTextBlock->SetText(FText::FromString(InUserInfo.Designation));
	}

	if (UserProfileImage != nullptr)
	{
		UAsyncTaskDownloadImage* DownloadedImageTask = NewObject<UAsyncTaskDownloadImage>();
		DownloadedImageTask->OnSuccess.AddDynamic(this, &UCallerNotifyProfileWidget::OnGetTexture);
		DownloadedImageTask->Start(InUserInfo.ProfileImageURL);
	}
}




void UCallerNotifyProfileWidget::OnGetTexture(UTexture2DDynamic* Texture)
{
	if (UserProfileImage == nullptr) return;
	UserProfileImage->SetBrushFromTextureDynamic(Texture, true);
}


void UCallerNotifyProfileWidget::OnResponseCallEvent(EMatrixCallEventType EventType, FString InRoomID, FMatrixCallEvent CallInfo)
{
	if (EventType == EMatrixCallEventType::Hangup)
	{
		RemoveFromParent();
	}
}



void UCallerNotifyProfileWidget::NativeDestruct()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().d_EventOnMatrixCallEventProcessed.RemoveAll(this);
		}
	}

	Super::NativeDestruct();
}

