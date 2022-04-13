// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu/NearbyChatWidget.h"
#include "UI/MainMenu/TeamInfoProfileBarWidget.h"
#include "Components/ScrollBox.h"

UNearbyChatWidget::UNearbyChatWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ShowProfileBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/TeamListProfileBar_wBP"));
	if (ShowProfileBPObj.Class != nullptr)
	{
		ProfileWidgetClass = ShowProfileBPObj.Class;
	}
}


void UNearbyChatWidget::SetupChannelInfo(FString InChannelName)
{
	ChannelName = InChannelName;

	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		IChannelSession& ChannelSession = TessClient->Getsession().GetChannelSessionFromKey(EVivoxChannelKey::NearByChannel);
		//ChannelSession.
		UpdateParticipants();

		if (&ChannelSession != nullptr)
		{
			ChannelSession.EventAfterParticipantAdded.AddUObject(this, &UNearbyChatWidget::OnVivoxChannelParticipantAdded);
			ChannelSession.EventBeforeParticipantRemoved.AddUObject(this, &UNearbyChatWidget::OnVivoxChannelParticipantRemove);
		}
	}

}

void UNearbyChatWidget::OnVivoxChannelParticipantAdded(const IParticipant& Participant)
{
	AddParticipant(Participant.Account().Name());
}


void UNearbyChatWidget::OnVivoxChannelParticipantRemove(const IParticipant& Participant)
{
	RemoveParticipant(Participant.Account().Name());
}

void UNearbyChatWidget::UpdateParticipants()
{
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

	if (TessClient != nullptr)
	{
		FTesserconUserInfo UserInfo = TessClient->Getsession().GetLoggedInUserInfo();
		LoggedInUserName = UserInfo.Username;
		IChannelSession& ChannelSession = TessClient->Getsession().GetChannelSessionFromKey(EVivoxChannelKey::NearByChannel);
		if (&ChannelSession != nullptr)
		{
			TMap<FString, IParticipant*> Participants = ChannelSession.Participants();
			for (auto It = Participants.CreateConstIterator(); It; ++It)
			{
				AddParticipant(It->Key);
			}
		}

	}
}

void UNearbyChatWidget::AddParticipant(FString UserID)
{
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		if (LoggedInUserName != UserID)
		{
			if (ProfileWidgetClass != nullptr)
			{
				TesserconAPI::FOnUserInfoByUserIDResponseDelegate  RequestListCallback;
				RequestListCallback.BindLambda([this, UserID](ETesserconError Error, FTesserconUserInfo UserInfo)
					{
						if (Error == ETesserconError::Success)
						{
							if (ProfileWidgetClass == nullptr)return;
							UTeamInfoProfileBarWidget* ProfileRow = CreateWidget<UTeamInfoProfileBarWidget>(GetWorld(), ProfileWidgetClass);
							if (ProfileRow == nullptr) return;
							ProfileRow->SetNearByProfile(UserInfo);
							ListScrollBox->AddChild(ProfileRow);
							WorldListMap.Add(UserID, ProfileRow);
						}
					});
				TessClient->Getsession().GetUserInfoByUsername(UserID, RequestListCallback);
			}
		}
	}
}

void UNearbyChatWidget::RemoveParticipant(FString UserID)
{
	UTeamInfoProfileBarWidget* WidgetToRemove;
	if (WorldListMap.RemoveAndCopyValue(UserID, WidgetToRemove))
	{
		if (WidgetToRemove != nullptr)
		{
			WidgetToRemove->RemoveFromParent();
		}
	}
}

void UNearbyChatWidget::NativeDestruct()
{
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		IChannelSession& ChannelSession = TessClient->Getsession().GetChannelSessionFromKey(EVivoxChannelKey::NearByChannel);
		if (&ChannelSession != nullptr)
		{
			ChannelSession.EventAfterParticipantAdded.RemoveAll(this);
			ChannelSession.EventBeforeParticipantRemoved.RemoveAll(this);
		}
	}
	Super::NativeDestruct();
}
