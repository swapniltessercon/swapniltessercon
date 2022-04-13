// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/TeamInfoWidget.h"

//class
#include "UI/MainMenu/TeamInfoProfileBarWidget.h"

//Components
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ComboBoxString.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"



UTeamInfoWidget::UTeamInfoWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> TeamUserProfileBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/TeamListProfileBar_wBP"));
	if (TeamUserProfileBPObj.Class != nullptr)
	{
		TeamUserProfileClass = TeamUserProfileBPObj.Class;
	}
}


void UTeamInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GroupInfoButton != nullptr)
	{
		GroupInfoButton->OnClicked.AddDynamic(this, &UTeamInfoWidget::OnGroupInfoButtonClicked);
	}
	if (MembersButton != nullptr)
	{
		MembersButton->OnClicked.AddDynamic(this, &UTeamInfoWidget::OnMembersButtonClicked);
	}
	if (AddMembersButton != nullptr)
	{
		AddMembersButton->OnClicked.AddDynamic(this, &UTeamInfoWidget::OnAddMembersButtonClicked);
	}
	if (LeaveGroupButton != nullptr)
	{
		LeaveGroupButton->OnClicked.AddDynamic(this, &UTeamInfoWidget::OnLeaveGroupButtonClicked);
	}
	if (SaveChangesButton != nullptr)
	{
		SaveChangesButton->OnClicked.AddDynamic(this, &UTeamInfoWidget::OnSaveChangesButtonClicked);
	}
	if (DeleteGroupButton != nullptr)
	{
		DeleteGroupButton->OnClicked.AddDynamic(this, &UTeamInfoWidget::OnDeleteGroupButtonClicked);
	}
	if (CancelButton != nullptr)
	{
		CancelButton->OnClicked.AddDynamic(this, &UTeamInfoWidget::OnCancelButtonClicked);
	}
	if (CloseButton != nullptr)
	{
		CloseButton->OnClicked.AddDynamic(this, &UTeamInfoWidget::RemoveFromParent);
	}
		
	if (TeamNameChangeEditableTextBox != nullptr)
	{
		TeamNameChangeEditableTextBox->OnTextChanged.AddDynamic(this, &UTeamInfoWidget::OnTeamNameEditableTextBoxChanged);
	}
	if (BGImage != nullptr)
	{
		BGImage->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDownImage"));
	}

}

FEventReply UTeamInfoWidget::OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent)
{
	RemoveFromParent();
	return FEventReply(false);
}

void UTeamInfoWidget::SetGroupInfo(ESelectedGroupInfo Type, TSharedPtr<MatrixRoomSession> InRoomSessionInfo)
{
	TeamRoomInfo=InRoomSessionInfo;


	//Get Join Team Memebrs
	TArray<FString> RoomMembers = TeamRoomInfo->GetMemberListForMembership(EMatrixMembership::Join);
	ShowTeamMembersList(RoomMembers);
	//


	if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
	{
		p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
	}

	//ComboBox
	switch (Type)
	{
	case ESelectedGroupInfo::GroupInfo:
		ShowGroupInfo();
		break;
	case ESelectedGroupInfo::Members:
		ShowMembers();
		break;
	case ESelectedGroupInfo::AddMembers:
		ShowAddMembers();
		break;
	default:
		break;
	}
}

void UTeamInfoWidget::OnGroupInfoButtonClicked()
{
	ShowGroupInfo();
}
void UTeamInfoWidget::OnMembersButtonClicked() 
{
	ShowMembers();
}
void UTeamInfoWidget::OnAddMembersButtonClicked()
{
	ShowAddMembers();
}

void UTeamInfoWidget::ShowGroupInfo()
{
	if (TeamRoomInfo->IsAdminOfThisRoom())
	{
		AdminButtonHorizontalBox->SetVisibility(ESlateVisibility::Visible);
		DeleteGroupButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		AdminButtonHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
		DeleteGroupButton->SetVisibility(ESlateVisibility::Collapsed);
		TeamNameChangeEditableTextBox->SetIsEnabled(false);
		DescriptionEditableTextBox->SetIsEnabled(false);
	}
	
	if (InfoWidgetSwitcher != nullptr)
	{
		TArray<FString>Admin = TeamRoomInfo->GetAdmins();
		FString Teamname = TeamRoomInfo->GetRoomName();
		TeamNameChangeEditableTextBox->SetText(FText::FromString(Teamname));
		int Length = Teamname.Len() - 1;
		TeamNameTextBlock->SetText(FText::FromString(Teamname.LeftChop(Length).ToUpper()));
		InfoWidgetSwitcher->SetActiveWidgetIndex(0);
	}
	

	
}

void UTeamInfoWidget::ShowMembers()
{ 
	if (InfoWidgetSwitcher != nullptr)
	{
		InfoWidgetSwitcher->SetActiveWidgetIndex(1);
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		if (TessClient != nullptr)
		{

			TessClient->Getsession().d_EventOnCallStateChanged.AddUObject(this, &UTeamInfoWidget::OnResponseCallstate);
		}
	}
	UpdateTeamParticipant();
}


//Update Call Participant
void UTeamInfoWidget::UpdateTeamParticipant()
{
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		FTesserconCallState CallInfo = TessClient->Getsession().GetCallState();
		if (CallInfo.CallStatus == ETesserconCallStatus::OnCall)
		{
			IChannelSession& ChannelSession = TessClient->Getsession().GetChannelSessionFromKey(EVivoxChannelKey::VoiceCallChannel);
			if (&ChannelSession != nullptr)
			{
				TMap<FString, IParticipant*> Participants = ChannelSession.Participants();

				for (auto It = Participants.CreateConstIterator(); It; ++It)
				{
					FString ParticipantUserID = It->Value->Account().Name();
					FString UserID = "@" + ParticipantUserID + ":comms.tessercon.com";
					if (DisplayProfileMap.Find(UserID))
					{
						UTeamInfoProfileBarWidget* Widget = *DisplayProfileMap.Find(UserID);
						if (Widget != nullptr)
						{
							Widget->SetSpeakerButtonVisible(ParticipantUserID);

						}
					}
				}
			}
		}
	}

}

//Get CallState Response
void UTeamInfoWidget::OnResponseCallstate(FTesserconCallState CallStateInfo)
{
	if (CallStateInfo.CallStatus == ETesserconCallStatus::OnCall)
	{
		
		if (TeamRoomInfo != nullptr)
		{
			if (CallStateInfo.RoomID == TeamRoomInfo->GetRoomID())
			{
				TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
				if (TessClient != nullptr)
				{
					IChannelSession& ChannelSession = TessClient->Getsession().GetChannelSessionFromKey(EVivoxChannelKey::VoiceCallChannel);
					if (&ChannelSession != nullptr)
					{
						ChannelSession.EventAfterParticipantAdded.AddUObject(this, &UTeamInfoWidget::OnVivoxChannelParticipantAdded);
						ChannelSession.EventBeforeParticipantRemoved.AddUObject(this, &UTeamInfoWidget::OnVivoxChannelParticipantRemove);

					}
				}
			}
		}
	}
	else if (CallStateInfo.CallStatus == ETesserconCallStatus::Available)
	{
		for (auto& Members : DisplayProfileMap)
		{
			UTeamInfoProfileBarWidget* Widget = Members.Value;
			if (Widget != nullptr)
			{
				Widget->SetSpeakerButtonCollapsed();
			}
		}
	}

}


//if Any one Join team Call then its set the Member List Speaker Visiblity as Visible
void UTeamInfoWidget::OnVivoxChannelParticipantAdded(const IParticipant& Participant)
{
	FString ParticipantUserID = Participant.Account().Name();
	FString UserID = "@" + ParticipantUserID + ":comms.tessercon.com";

	if (DisplayProfileMap.Find(UserID))
	{
		UE_LOG(LogTemp, Warning, TEXT("UTeamChatWindowWidget::OnVivoxChannelParticipantAdded"));
		UTeamInfoProfileBarWidget* Widget = *DisplayProfileMap.Find(UserID);
		if (Widget != nullptr)
		{
			Widget->SetSpeakerButtonVisible(ParticipantUserID);
		}
	}
}
///

//if Any one Leave team Call then its set the Member List Speaker Visiblity as Collapsed
void UTeamInfoWidget::OnVivoxChannelParticipantRemove(const IParticipant& Participant)
{
	FString UserID = Participant.Account().Name();
	UserID = "@" + UserID + ":comms.tessercon.com";

	if (DisplayProfileMap.Find(UserID))
	{
		UTeamInfoProfileBarWidget* Widget = *DisplayProfileMap.Find(UserID);

		if (Widget != nullptr)
		{
			Widget->SetSpeakerButtonCollapsed();
		}
	}
}
///


void UTeamInfoWidget::ShowTeamMembersList(TArray<FString> InRoomMembers)
{
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		for (auto Member : InRoomMembers)
		{
			FString Username = GetUsernameFromMatrixUsername(Member);
			FTesserconUserInfo LoggedUserInfo = TessClient->Getsession().GetLoggedInUserInfo();
			if (Username != LoggedUserInfo.Username)
			{
				TesserconAPI::FOnUserInfoByUserIDResponseDelegate  RequestListCallback;
				RequestListCallback.BindLambda([this, Member](ETesserconError Error, FTesserconUserInfo UserInfo)
					{
						
						if (Error == ETesserconError::Success)
						{
							if (!DisplayProfileMap.Contains(Member))
							{
								if (TeamUserProfileClass == nullptr)return;
								UTeamInfoProfileBarWidget* ProfileRow = CreateWidget<UTeamInfoProfileBarWidget>(GetWorld(), TeamUserProfileClass);
								if (ProfileRow == nullptr) return;
								ProfileRow->SetTeamMemberProfile(UserInfo);
								MembersListScrollBox->SetScrollOffset(0);
								MembersListScrollBox->AddChild(ProfileRow);
								DisplayProfileMap.Add(Member, ProfileRow);
							}
						}
					});
				TessClient->Getsession().GetUserInfoByUsername(Username, RequestListCallback);
			}
		}
	
	}
}


//

void UTeamInfoWidget::ShowAddMembers()
{

	if (InfoWidgetSwitcher != nullptr)
	{
		AddMembersScrollBox->ClearChildren();
		InfoWidgetSwitcher->SetActiveWidgetIndex(2);
		SortFriendListForAddMembers();
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		if (TessClient != nullptr)
		{
			for (auto RoomPair : SortedFriendList)
			{
				FString Username = RoomPair.Key;
				TesserconAPI::FOnUserInfoByUserIDResponseDelegate RequestListCallback;
				RequestListCallback.BindLambda([this](ETesserconError Error, FTesserconUserInfo UserInfo)
					{
						if (Error == ETesserconError::Success)
						{
							if (TeamUserProfileClass == nullptr)return;
							UTeamInfoProfileBarWidget* ProfileRow = CreateWidget<UTeamInfoProfileBarWidget>(GetWorld(), TeamUserProfileClass);
							if (ProfileRow == nullptr) return;
							//TODO::Pass With Membership
							ProfileRow->SetAddMembersProfile(UserInfo, TeamRoomInfo);
							AddMembersScrollBox->AddChild(ProfileRow);
						}
					});
				TessClient->Getsession().GetUserInfoByUsername(Username, RequestListCallback);
			}
		}
	}
}


//Leave room 
void UTeamInfoWidget::OnLeaveGroupButtonClicked()
{
	if (p_ChatClient != nullptr)
	{
		MatrixNativeAPI::FOnMatrixAPIResponseDelegate LeaveTeamResponceCallback;
		LeaveTeamResponceCallback.BindLambda([this](EMatrixCoreError Error)
			{ if (Error == EMatrixCoreError::Success)
			 {
				RemoveFromParent();
			 }
			});
		p_ChatClient->GetSession().LeaveRoom(TeamRoomInfo->GetRoomID(), false, LeaveTeamResponceCallback);
	}
}

void UTeamInfoWidget::OnSaveChangesButtonClicked()
{
	FString UpdateTeamName = TeamNameChangeEditableTextBox->GetText().ToString();
	if (!UpdateTeamName.IsEmpty())
	{
		if (p_ChatClient != nullptr)
		{
			MatrixNativeAPI::FOnMatrixAPIResponseDelegate ChangeTeamNameCallBack;
			ChangeTeamNameCallBack.BindLambda([this, UpdateTeamName](EMatrixCoreError Error)
				{
					if (Error == EMatrixCoreError::Success)
					{
						RemoveFromParent();
					}
				});
			p_ChatClient->GetSession().UpdateRoomName(TeamRoomInfo->GetRoomID(), UpdateTeamName, ChangeTeamNameCallBack);
		}
	}

}


void UTeamInfoWidget::OnDeleteGroupButtonClicked()
{
	//TODO::
}

void UTeamInfoWidget::OnCancelButtonClicked()
{
	this->RemoveFromParent();
}



void UTeamInfoWidget::OnTeamNameEditableTextBoxChanged(const FText& text)
{
	FText Text = ClampText(text, 20);
	TeamNameChangeEditableTextBox->SetText(FText(Text));
}

FText UTeamInfoWidget::ClampText(FText Text, int32 MaxCount)
{
	FString Teamname = Text.ToString();
	if (Teamname.Len() > MaxCount)
	{
		FString StringFormat = Teamname.LeftChop(1);
		FText TextFormat = FText::FromString(StringFormat);
		return TextFormat;
	}
	else
	{
		return Text;
	}
}




void UTeamInfoWidget::SortFriendListForAddMembers()
{
	if (p_ChatClient != nullptr && TeamRoomInfo != nullptr)
	{
		TArray<FString> JoinTeamMembers = TeamRoomInfo->GetMemberListForMembership(EMatrixMembership::Join);
		TMap<FString, TSharedPtr<MatrixRoomSession>> FriendList = p_ChatClient->GetSession().GetFriendRooms();

		for (auto RoomInfo : FriendList)
		{
			FString UserID = p_ChatClient->GetSession().GetFriendUserIDForRoom(RoomInfo.Key);
			FString Username = GetUsernameFromMatrixUsername(UserID);
			SortedFriendList.Add(Username, RoomInfo.Value);
		}
		for (auto Member : JoinTeamMembers)
		{
			FString Username = GetUsernameFromMatrixUsername(Member);
			if (SortedFriendList.Contains(Username))
			{
				SortedFriendList.FindAndRemoveChecked(Username);
			}
		}
	}
}
