// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/TeamInfoProfileBarWidget.h"

//class 
#include "UI/MainMenu/TeamCreateWidget.h"
#include "UI/MainMenu/TeamInfoWidget.h"

#include "UI/Profile/UserProfileWidget.h"



//Download
#include "Blueprint/AsyncTaskDownloadImage.h"

// Components
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ComboBoxString.h"



void UTeamInfoProfileBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ToggleButton != nullptr)
	{
		ToggleButton->OnClicked.AddDynamic(this, &UTeamInfoProfileBarWidget::OnToggleButtonClicked);
	}
	if (SpeakerButton != nullptr)
	{
		SpeakerButton->OnClicked.AddDynamic(this, &UTeamInfoProfileBarWidget::OnSpeakerButtonClicked);
	}
	if (ProfileInfoComboBoxString != nullptr)
	{
		
		ProfileInfoComboBoxString->OnSelectionChanged.AddDynamic(this, &UTeamInfoProfileBarWidget::OnSelectOption);
	}
}
void UTeamInfoProfileBarWidget::PopulateComboBox()
{
	TArray<FString> InfoListBox;
	InfoListBox.Add("Profile");
	InfoListBox.Add("Request BusinessCard");
	InfoListBox.Add("Block");
	for (auto It : InfoListBox)
	{
		ProfileInfoComboBoxString->AddOption(It);
	}
}
void UTeamInfoProfileBarWidget::PopulateTeamMemberInfoComboBox()
{
	TArray<FString> InfoListBox;
	InfoListBox.Add("Profile");
	InfoListBox.Add("Start Chat");
	InfoListBox.Add("Request BusinessCard");
	InfoListBox.Add("Remove");
	InfoListBox.Add("Block");
	for (auto It : InfoListBox)
	{
		ProfileInfoComboBoxString->AddOption(It);
	}
}
void UTeamInfoProfileBarWidget::SetProfile(UTeamCreateWidget* InParent, FTesserconUserInfo InUserInfo)
{
	PopulateComboBox();
	Parent=InParent;
	UserInfo = InUserInfo;
	SetProfileInfo();
}

void UTeamInfoProfileBarWidget::SetAddMembersProfile(FTesserconUserInfo InUserInfo, TSharedPtr<MatrixRoomSession> InRoomSessionInfo)
{
	TeamRoomInfo=InRoomSessionInfo;
	UserInfo = InUserInfo;
	PopulateTeamMemberInfoComboBox();
	//TODO :: here Manage Invite and Cancel
	ToggleTextBlock->SetText(FText::FromString("Invite"));
	SetProfileInfo();
}


void UTeamInfoProfileBarWidget::SetTeamMemberProfile(FTesserconUserInfo InUserInfo)
{
	ChannelKey = EVivoxChannelKey::VoiceCallChannel;
	UserInfo = InUserInfo;
	ToggleTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	//SpeakerButton->SetVisibility(ESlateVisibility::Visible);
	SetProfileInfo();
	PopulateTeamMemberInfoComboBox();
}


void UTeamInfoProfileBarWidget::SetSearchMemberProfile(FTesserconUserInfo InUserInfo)
{
	UserInfo = InUserInfo;
	SetProfileInfo();
	TArray<FString> InfoListBox;
	ToggleTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	InfoListBox.Add("Start Chat");
	InfoListBox.Add("Call");
	
	for (auto It : InfoListBox)
	{
		ProfileInfoComboBoxString->AddOption(It);
	}
}


void UTeamInfoProfileBarWidget::SetNearByProfile(FTesserconUserInfo InUserInfo)
{

	ChannelKey = EVivoxChannelKey::NearByChannel;
	UserInfo = InUserInfo;
	SetProfileInfo();
	TArray<FString> InfoListBox;
	ToggleButton->SetVisibility(ESlateVisibility::Collapsed);
	SpeakerButton->SetVisibility(ESlateVisibility::Visible);
	InfoListBox.Add("Profile");
	InfoListBox.Add("Start Chat");
	InfoListBox.Add("Request BusinessCard");
	for (auto It : InfoListBox)
	{
		ProfileInfoComboBoxString->AddOption(It);
	}
}

void UTeamInfoProfileBarWidget::SetProfileInfo()
{
	if (UserNameTextBlock != nullptr)
	{
		FString Name = UserInfo.FirstName + " " + UserInfo.LastName;
		UserNameTextBlock->SetText(FText::FromString(Name));
	}
	if (DesignationTextBlock != nullptr)
	{
		DesignationTextBlock->SetText(FText::FromString(UserInfo.Designation));
	}
	if (ProfileImage != nullptr)
	{
		UAsyncTaskDownloadImage* DownloadedImageTask = NewObject<UAsyncTaskDownloadImage>();
		DownloadedImageTask->OnSuccess.AddDynamic(this, &UTeamInfoProfileBarWidget::OnGetTexture);
		DownloadedImageTask->Start(UserInfo.ProfileImageURL);
	}
}
void UTeamInfoProfileBarWidget::SetToggleStatus()
{
	//TODO::
}
void UTeamInfoProfileBarWidget::OnGetTexture(UTexture2DDynamic* Texture)
{
	if (ProfileImage == nullptr) return;
	ProfileImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	ProfileImage->SetBrushFromTextureDynamic(Texture, true);
}
void UTeamInfoProfileBarWidget::OnToggleButtonClicked()
{
	FString Status = ToggleTextBlock->GetText().ToString();
	if (Parent != nullptr)
	{
		if (Status == "Add")
		{
			Parent->AddInTeamList(UserInfo);
			ToggleTextBlock->SetText(FText::FromString("Cancel"));
		}
		else if (Status == "Cancel")
		{
			Parent->AddInTeamList(UserInfo);
			ToggleTextBlock->SetText(FText::FromString("Add"));
		}
	}
	else
	{
		if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
		{
			p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();

			if (Status == "Invite")
			{
				if (p_ChatClient != nullptr)
				{
					FString UserID = GetMatrixFullUserIDForUsername(UserInfo.Username);
					UE_LOG(LogTemp, Warning, TEXT("UTeamChatWindowWidget::OnADDMembersButtonClicked %s"), *UserID);
					p_ChatClient->GetSession().InviteUserToRoom(TeamRoomInfo->GetRoomID(), UserID);
					ToggleTextBlock->SetText(FText::FromString("Cancel"));
				}
			}
			else if (Status == "Cancel")
			{
				//Matrix User ID
				FString UserID = GetMatrixFullUserIDForUsername(UserInfo.Username);
				if (p_ChatClient != nullptr)
				{
					MatrixNativeAPI::FOnMatrixAPIResponseDelegate MemberRemoveFromTeamCallBack;
					MemberRemoveFromTeamCallBack.BindLambda([this](EMatrixCoreError Error)
						{
							if (EMatrixCoreError::Success == Error)
							{
								ToggleTextBlock->SetText(FText::FromString("Invite"));
							}
						});

					p_ChatClient->GetSession().KickUserFromRoom(TeamRoomInfo->GetRoomID(), UserID, MemberRemoveFromTeamCallBack);
				}
			}
		}
	}
}
void UTeamInfoProfileBarWidget::OnSpeakerButtonClicked()
{
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		
		IChannelSession& ChannelSession = TessClient->Getsession().GetChannelSessionFromKey(ChannelKey);
		if (&ChannelSession != nullptr)
		{
			TMap<FString, IParticipant*> ParticipantList = ChannelSession.Participants();
			//Vivox User ID
			FString UserID = UserInfo.Username;
			IParticipant* Member = *ParticipantList.Find(UserID);
			if (bIsMicEnable)
			{
				if (Member != nullptr)
				{
					Member->SetLocalMute(true);
					bIsMicEnable = false;

				}
			}
			else
			{
				if (Member != nullptr)
				{
					Member->SetLocalMute(false);
					bIsMicEnable = true;
				}

			}
		}
	}
}

void UTeamInfoProfileBarWidget::OnSelectOption(FString SelectedOption, ESelectInfo::Type SelectType)
{
	ProfileInfoComboBoxString->ClearSelection();
	if (SelectedOption == "Profile")
	{
		ShowProfile();
	}
	else if (SelectedOption == "Request BusinessCard")
	{
		//TODO::Request Buiness Card
	}
	else if (SelectedOption == "Block")
	{
		//TODO::Block
	}
	else if (SelectedOption == "Remove")
	{
		//TODO::Remove Member
	}
	else if (SelectedOption == "Start Chat")
	{
		ShowChatWindowWidget();
	}
}


void UTeamInfoProfileBarWidget::ShowProfile()
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
				FString UserID=GetMatrixFullUserIDForUsername(UserInfo.Username);
				UserProfileWidget->SetProfile(UserInfo, UserID);
				MenuWidget->SetBlankSizeBox(UserProfileWidget);
			}
		}
	}
}

//Click On Chat Button ist Shows The  Chat Widget
void UTeamInfoProfileBarWidget::ShowChatWindowWidget()
{
	if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
	{
		p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
		FString UserID = GetMatrixFullUserIDForUsername(UserInfo.Username);
		FString RoomID = p_ChatClient->GetSession().GetRoomIDForFriend(UserID);
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController != nullptr)
		{
			ABaseHUD* HUD = Cast<ABaseHUD>(PlayerController->GetHUD());
			if (HUD != nullptr)
			{
				UBaseMenuWidget* MenuWidget = Cast<UBaseMenuWidget>(HUD->GetCurrentMenuWidget());
				if (MenuWidget != nullptr)
				{
					TMap<FString, TSharedPtr<MatrixRoomSession>> FriendRoomSession = p_ChatClient->GetSession().GetFriendRooms();
					TSubclassOf<UUserWidget> ProfileBarClass;
					MenuWidget->GetProfileBarClass(ProfileBarClass);
					UProfileBarWidget* ProfileRow = CreateWidget<UProfileBarWidget>(GetWorld(), ProfileBarClass);
					if (ProfileRow != nullptr)
					{
						if (FriendRoomSession.Find(RoomID))
						{
							ProfileRow->ShowChatWidget(UserInfo, RoomID);
						}
					}
				}
			}
		}
	}
}



void UTeamInfoProfileBarWidget::SetSpeakerButtonCollapsed()
{
	SpeakerButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UTeamInfoProfileBarWidget::SetSpeakerButtonVisible(FString InParticipantUserID)
{
	if (SpeakerButton != nullptr)
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		IChannelSession& ChannelSession = TessClient->Getsession().GetChannelSessionFromKey(EVivoxChannelKey::VoiceCallChannel);
		if (&ChannelSession != nullptr)
		{
			TMap<FString, IParticipant*> ParticipantList = ChannelSession.Participants();
			IParticipant* Member = *ParticipantList.Find(InParticipantUserID);
			if (Member->LocalMute())
			{
				SetSpeakerMuteButtonImage();
			}
			else
			{
				SetUserSpeakerButtonImage();
			}
		}
		SpeakerButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTeamInfoProfileBarWidget::SetUserSpeakerButtonImage()
{
	FString ImageUrlStr = FString::Printf(TEXT("/Game/BaseCore/Textures/MainMenu/CreateTeam/CreateSpeakerDef"));
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(ImageUrlStr)));
	FSlateBrush Brush;
	FString HoverdImageUrlStr = FString::Printf(TEXT("/Game/BaseCore/Textures/MainMenu/CreateTeam/CreateSpeakerDef"));
	UTexture2D* HoverdTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(HoverdImageUrlStr)));
	FSlateBrush HoverdBrush;
	if (Texture != nullptr)
	{
		Brush.SetResourceObject(Texture);
		HoverdBrush.SetResourceObject(HoverdTexture);
		SpeakerButton->WidgetStyle.SetNormal(Brush);
		SpeakerButton->WidgetStyle.SetHovered(HoverdBrush);
		bIsMicEnable = true;
	}

}

void UTeamInfoProfileBarWidget::SetSpeakerMuteButtonImage()
{

	//TODO:Change Image Path
	FString ImageUrlStr = FString::Printf(TEXT("/Game/BaseCore/Textures/MainMenu/CallBar/MainMenuSpeakerOffHover"));
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(ImageUrlStr)));
	FSlateBrush Brush;
	if (Texture != nullptr)
	{
		Brush.SetResourceObject(Texture);
		SpeakerButton->WidgetStyle.SetNormal(Brush);
		SpeakerButton->WidgetStyle.SetHovered(Brush);
		bIsMicEnable = false;
	}
}

