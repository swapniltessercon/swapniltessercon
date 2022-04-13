// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/ChatWindowWidget.h"
//Download
#include "Blueprint/AsyncTaskDownloadImage.h"
//class
#include "UI/Profile/UserProfileWidget.h"
#include "UI/MainMenu/TeamInfoWidget.h"
//Components
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ComboBoxString.h"
#include "Components/WidgetSwitcher.h"


void UChatWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton != nullptr)
	{
		BackButton->OnClicked.AddDynamic(this, &UChatWindowWidget::OnBackButtonClicked);
	}

	if (InfoListComboBoxString != nullptr)
	{
		InfoListComboBoxString->OnSelectionChanged.AddDynamic(this, &UChatWindowWidget::OnSelectOption);
	}

	if (RejectButton != nullptr)
	{
		RejectButton->OnClicked.AddDynamic(this, &UChatWindowWidget::OnRejectButtonClicked);
	}

	if (AcceptButton != nullptr)
	{
		AcceptButton->OnClicked.AddDynamic(this, &UChatWindowWidget::OnAcceptButtonClicked);
	}



	//TODO::Write a Code in Base Class

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		TessClient->Getsession().d_EventOnMicAvailablityChanged.AddUObject(this, &UChatWindowWidget::OnMicAvailablityChanged);
		TessClient->Getsession().d_EventOnSpeakerAvailablityChanged.AddUObject(this, &UChatWindowWidget::OnSpeakerAvailablityChanged);

		/*if (!TessClient->Getsession().IsMicAvailable())
		{
			MicNotifyImage->SetVisibility(ESlateVisibility::Visible);
			CallButton->SetIsEnabled(false);
		}
		else
		{
			MicNotifyImage->SetVisibility(ESlateVisibility::Collapsed);
			CallButton->SetIsEnabled(true);
		}*/
		
	}
	
}

void UChatWindowWidget::OnSpeakerAvailablityChanged(bool bIsSpeakerAvailable)
{

	if (!bIsSpeakerAvailable)
	{
		MicNotifyImage->SetVisibility(ESlateVisibility::Visible);
		CallButton->SetIsEnabled(false);
	}
	else
	{
		MicNotifyImage->SetVisibility(ESlateVisibility::Collapsed);
		CallButton->SetIsEnabled(true);
	}

}
void UChatWindowWidget::OnMicAvailablityChanged(bool bIsMicAvailable)
{

	if (!bIsMicAvailable)
	{
		MicNotifyImage->SetVisibility(ESlateVisibility::Visible);
		CallButton->SetIsEnabled(false);
	}
	else
	{
		MicNotifyImage->SetVisibility(ESlateVisibility::Collapsed);
		CallButton->SetIsEnabled(true);
	}

}

void UChatWindowWidget::PopulateComboBox()
{ 
	TArray<FString> FriednInfoList;
	FriednInfoList.Add("Profile");
	FriednInfoList.Add("Request BusinessCard");
	FriednInfoList.Add("Block");
	for (auto It : FriednInfoList)
	{
		InfoListComboBoxString->AddOption(It);
	}
}

void UChatWindowWidget::PopulateTeamComboBox()
{
	TArray<FString> TeamInfoList;
	TeamInfoList.Add("Group Info");
	TeamInfoList.Add("Members");
	TeamInfoList.Add("Add Members");
	TeamInfoList.Add("Leave Group");
	TeamInfoList.Add("Delete Group");
	for (auto It : TeamInfoList)
	{
		InfoListComboBoxString->AddOption(It);
	}
}

void UChatWindowWidget::FriendChatWidgetSetup(FTesserconUserInfo InUserInfo ,FString InRoomID, bool bIsRequest, UProfileBarWidget* InParent)
{
	Parent = InParent;
	RoomID = InRoomID;
	UserInfo = InUserInfo;
	BaseSetup(InRoomID,true);
	PopulateComboBox();
	SetFriendProfileInfo(InUserInfo);
	if (bIsRequest)
	{
		InfoListComboBoxString->SetVisibility(ESlateVisibility::Collapsed);
		ChatWidgetSwitcher->SetActiveWidgetIndex(1);
		SetFriendRequestProfile(InUserInfo);
		
	}
}

void UChatWindowWidget::TeamChatWidgetSetup(TSharedPtr<MatrixRoomSession> InRoomSessionInfo, UProfileBarWidget* InParent)
{
	TeamRoomInfo = InRoomSessionInfo;
	Parent = InParent;
	RoomID = TeamRoomInfo->GetRoomID();
	PopulateTeamComboBox();
	BaseSetup(RoomID,false);
	SetTeamProfileInfo();
	CreateTeamInfoWidget();
	if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
	{
		p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
		if (p_ChatClient != nullptr)
		{
			m_ChatRoomSession = p_ChatClient->GetSession().GetRoomSession(RoomID);
			m_ChatRoomSession->d_EventOnRoomNameChanged.AddUObject(this, &UChatWindowWidget::TeamNameChangeResponse);
		}
	}
}

void UChatWindowWidget::TeamNameChangeResponse(FString TeamName)
{
	int Length = TeamName.Len() - 1;
	if (UsernameTextBlock != nullptr)
	{
		UsernameTextBlock->SetText(FText::FromString(TeamName));
	}
	if (TeamTextBlock != nullptr)
	{
		TeamTextBlock->SetText(FText::FromString(TeamName.LeftChop(Length).ToUpper()));
	}

}

void UChatWindowWidget::CreateTeamInfoWidget() 
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController != nullptr)
	{
		ABaseHUD* HUD = Cast<ABaseHUD>(PlayerController->GetHUD());
		if (HUD != nullptr)
		{
			MenuWidget = Cast<UBaseMenuWidget>(HUD->GetCurrentMenuWidget());
			if (TeamInfoClass != nullptr && MenuWidget != nullptr)
			{
				TeamInfoWidget = CreateWidget<UTeamInfoWidget>(GetWorld(), TeamInfoClass);
			}
		}
	}
}

void UChatWindowWidget::OnSelectOption(FString SelectedOption, ESelectInfo::Type SelectType)
{
	InfoListComboBoxString->ClearSelection();

	if (SelectedOption == "Profile")
	{
		ShowProfile();
	}
	else if (SelectedOption == "Request BusinessCard")
	{
		//TODO:
	}
	else if (SelectedOption == "Block")
	{
		//TODO:
	}
	else if (SelectedOption == "Group Info")
	{
		ShowGroupInfo(ESelectedGroupInfo::GroupInfo);
	}
	else if (SelectedOption == "Members")
	{
		ShowGroupInfo(ESelectedGroupInfo::Members);
	}
	else if (SelectedOption == "Add Members")
	{
		ShowGroupInfo(ESelectedGroupInfo::AddMembers);
	}
	else if (SelectedOption == "Leave Group")
	{
		//TODO:
	}
	else if (SelectedOption == "Delete Group")
	{
		//TODO:
	}
}

void UChatWindowWidget::OnBackButtonClicked()
{
	if (Parent != nullptr)
	{
		Parent->SetBackButton();
		RemoveFromParent();
	}
}

//Reject Frined Request Remove the Chat Widget and display the Chat List 
void UChatWindowWidget::OnRejectButtonClicked()
{
	//TODO:
	/*if (p_ChatClient != nullptr)
	{
		MatrixNativeAPI::FOnMatrixAPIResponseDelegate LeaveRoomResponseCallback;
		LeaveRoomResponseCallback.BindLambda([this](EMatrixCoreError Error) {
			if (Error == EMatrixCoreError::Success)
			{
				if (Parent != nullptr)
				{

					RemoveFromParent();
					Parent->SetBackButton();
				}
			}
			});
		p_ChatClient->GetSession().LeaveRoom(RoomID, true, LeaveRoomResponseCallback);
	}*/

	RemoveFromParent();
	Parent->SetBackButton();
}

//Accept Frined Request then switch the Widget [and also Visible ComboBox]. 
void UChatWindowWidget::OnAcceptButtonClicked()
{

	//TODO:
	//Sender Name its needed To join Room 
	/*FString Sender = GetMatrixFullUserIDForUsername(UserInfo.Username);
	if (p_ChatClient != nullptr)
	{
		MatrixNativeAPI::FOnMatrixAPIResponseDelegate AcceptRoomResponseCallback;
		AcceptRoomResponseCallback.BindLambda([this](EMatrixCoreError Error) {
			if (Error == EMatrixCoreError::Success)
			{
				ChatWidgetSwitcher->SetActiveWidgetIndex(0);
				InfoListComboBoxString->SetVisibility(ESlateVisibility::Visible);
			}
			});
		p_ChatClient->GetSession().JoinRoom(RoomID, true, Sender, AcceptRoomResponseCallback);
	}*/

	ChatWidgetSwitcher->SetActiveWidgetIndex(0);
	InfoListComboBoxString->SetVisibility(ESlateVisibility::Visible);
}

void UChatWindowWidget::ShowProfile()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController != nullptr)
	{
		ABaseHUD* HUD = Cast<ABaseHUD>(PlayerController->GetHUD());

		if (HUD != nullptr)
		{
			MenuWidget = Cast<UBaseMenuWidget>(HUD->GetCurrentMenuWidget());
			UUserProfileWidget* UserProfileWidget = CreateWidget<UUserProfileWidget>(GetWorld(), UserProfileClass);
			if (MenuWidget != nullptr && UserProfileWidget != nullptr)
			{
				FString UserID = GetMatrixFullUserIDForUsername(UserInfo.Username);
				UserProfileWidget->SetProfile(UserInfo, UserID);
				MenuWidget->SetBlankSizeBox(UserProfileWidget);
			}
		}
	}
}

void UChatWindowWidget::ShowGroupInfo(ESelectedGroupInfo Type)
{
	if (TeamInfoWidget != nullptr)
	{
		if (MenuWidget != nullptr)
		{
			MenuWidget->SetBlankSizeBox(TeamInfoWidget);
			TeamInfoWidget->SetGroupInfo(Type, TeamRoomInfo);
		}
	}
}

void UChatWindowWidget::SetFriendProfileInfo(FTesserconUserInfo InUserInfo)
{
	if (UsernameTextBlock != nullptr)
	{
		FString Name = InUserInfo.FirstName + " " + InUserInfo.LastName;
		UsernameTextBlock->SetText(FText::FromString(Name));
	}
	if (DesignationTextBlock != nullptr)
	{
		DesignationTextBlock->SetText(FText::FromString(InUserInfo.Designation));
	}
	if (ProfileImage != nullptr)
	{
		UAsyncTaskDownloadImage* DownloadedImageTask = NewObject<UAsyncTaskDownloadImage>();
		DownloadedImageTask->OnSuccess.AddDynamic(this, &UChatWindowWidget::OnGetTexture);
		DownloadedImageTask->Start(InUserInfo.ProfileImageURL);
	}
}

void UChatWindowWidget::SetTeamProfileInfo()
{
	//TArray<FString>Admin = TeamRoomInfo->GetAdmins();
	FString TeamName = TeamRoomInfo->GetRoomName();
	int Length = TeamName.Len() - 1;
	if (UsernameTextBlock != nullptr)
	{
		UsernameTextBlock->SetText(FText::FromString(TeamName));
	}
	if (TeamTextBlock != nullptr)
	{
		TeamTextBlock->SetText(FText::FromString(TeamName.LeftChop(Length).ToUpper()));
	}
}

void UChatWindowWidget::SetFriendRequestProfile(FTesserconUserInfo InUserInfo)
{
	if (RequestNameTextBlock != nullptr)
	{
		FString Name = InUserInfo.FirstName + " " + InUserInfo.LastName;
		RequestNameTextBlock->SetText(FText::FromString(Name));
	}
	if (RequestDesignationTextBlock != nullptr)
	{
		RequestDesignationTextBlock->SetText(FText::FromString(InUserInfo.Designation));
	}
	if (RequestProfileImage != nullptr)
	{
		UAsyncTaskDownloadImage* DownloadedImageTask = NewObject<UAsyncTaskDownloadImage>();
		DownloadedImageTask->OnSuccess.AddDynamic(this, &UChatWindowWidget::OnGetRequetProfileTexture);
		DownloadedImageTask->Start(InUserInfo.ProfileImageURL);
	}
}

void UChatWindowWidget::OnGetRequetProfileTexture(UTexture2DDynamic* Texture)
{
	if (RequestProfileImage == nullptr) return;
	RequestProfileImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	RequestProfileImage->SetBrushFromTextureDynamic(Texture, true);

}

void UChatWindowWidget::OnGetTexture(UTexture2DDynamic* Texture)
{
	if (ProfileImage == nullptr) return;
	ProfileImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	ProfileImage->SetBrushFromTextureDynamic(Texture, true);
}

void UChatWindowWidget::NativeDestruct()
{
	Super::NativeDestruct();
	InfoListComboBoxString->OnSelectionChanged.RemoveAll(this);
}