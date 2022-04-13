// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/ListOfChatUserWidget.h"

//Class
#include "UI/MainMenu/SearchNewUserWidget.h"
#include "UI/MainMenu/MainMenuWidget.h"
#include "UI/HUD/MainMenuHUD.h"
#include "UI/MainMenu/ProfileBarWidget.h"
#include "UI/MainMenu/TeamCreateWidget.h"

//Components
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


UListOfChatUserWidget::UListOfChatUserWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> SearchUserBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/SearchUser_WBP"));
	if (SearchUserBPObj.Class != nullptr)
	{
		SearchUserClass = SearchUserBPObj.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> ProfileBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/ProfileBar_WBP"));
	if (ProfileBPObj.Class != nullptr)
	{
		ProfileBarClass = ProfileBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> CreateTeamWidgetBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/CreateTeam_WBP"));
	if (CreateTeamWidgetBPObj.Class != nullptr)
	{
		CreateTeamWidgetClass = CreateTeamWidgetBPObj.Class;
	}
}



void UListOfChatUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SearchEditableTextBox != nullptr)
	{
		SearchEditableTextBox->OnTextChanged.AddDynamic(this, &UListOfChatUserWidget::OnSearchEditableTextBoxClicked);
	}
	if (CreateNewTeamButton != nullptr)
	{
		CreateNewTeamButton->OnClicked.AddDynamic(this, &UListOfChatUserWidget::OnCreateNewTeamButtonClicked);
	}
	if (NewFriendAddButton != nullptr)
	{
		NewFriendAddButton->OnClicked.AddDynamic(this, &UListOfChatUserWidget::OnNewFriendAddButtonClicked);
	}
}

void UListOfChatUserWidget::Setup(EChatType ChatType)
{
	if (ChatType == EChatType::Single)
	{
		if(HeadingTextBlock != nullptr)
			HeadingTextBlock->SetText(FText::FromString("Direct Message"));

		if(!NewFriendAddButton->IsVisible())
			NewFriendAddButton->SetVisibility(ESlateVisibility::Visible);

		if(!CreateNewTeamButton->IsVisible())
			CreateNewTeamButton->SetVisibility(ESlateVisibility::Collapsed);
		
		if(FModuleManager::Get().IsModuleLoaded("MatrixCore"))
		{
			p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
		}
		if (p_ChatClient != nullptr)
		{
			p_ChatClient->GetSession().d_EventOnFriendAdded.AddUObject(this, &UListOfChatUserWidget::OnFriendAddedResponce);
			p_ChatClient->GetSession().d_EventOnFriendRemoved.AddUObject(this, &UListOfChatUserWidget::OnFriendRemoveResponce);
			p_ChatClient->GetSession().d_EventOnFriendRequestReceived.AddUObject(this, &UListOfChatUserWidget::OnFriendRequestRecived);
		}
		ListScrollBox->ClearChildren();
		FriendDisplayRoomsMap.Empty();
		UpdateFriendChanges();
		UpdateFriendRequestList();
	}
	else if (ChatType == EChatType::Group)
	{
		if (HeadingTextBlock != nullptr)
			HeadingTextBlock->SetText(FText::FromString("Group"));
		if(!CreateNewTeamButton->IsVisible())
			CreateNewTeamButton->SetVisibility(ESlateVisibility::Visible);
		if (!NewFriendAddButton->IsVisible())
			NewFriendAddButton->SetVisibility(ESlateVisibility::Collapsed);

		if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
		{
			p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
		}
		if (p_ChatClient != nullptr)
		{
			p_ChatClient->GetSession().d_EventOnTeamAdded.AddUObject(this, &UListOfChatUserWidget::OnTeamAdded);
			p_ChatClient->GetSession().d_EventOnTeamRemoved.AddUObject(this, &UListOfChatUserWidget::OnTeamRemove);
			p_ChatClient->GetSession().d_EventOnTeamRequestReceived.AddUObject(this, &UListOfChatUserWidget::OnTeamRequestRecived);
		}
		ListScrollBox->ClearChildren();
		TeamDisplayRoomsMap.Empty();
		UpdateTeamChanges();
		UpdateTeamRequestList();
	}
}



//Update Friend List On Event Response
void UListOfChatUserWidget::OnFriendAddedResponce(FString  FriendID)
{
	if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
	{
		p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
		if (p_ChatClient != nullptr)
		{
			FString RoomID = p_ChatClient->GetSession().GetRoomIDForFriend(FriendID);
			AddFriendInList(RoomID, false);
		}
	}
}


//Remove From Friend List
void UListOfChatUserWidget::OnFriendRemoveResponce(FString  FriendID)
{
	RemoveFriendFromList(FriendID);
}

void UListOfChatUserWidget::RemoveFriendFromList(FString FriendID)
{
	if (p_ChatClient != nullptr)
	{
		FString RoomID = p_ChatClient->GetSession().GetRoomIDForFriend(FriendID);
		if (FriendDisplayRoomsMap.Find(RoomID))
		{
			UProfileBarWidget* RemoveWidget = FriendDisplayRoomsMap.FindAndRemoveChecked(RoomID);
			if (RemoveWidget != nullptr)
			{
				RemoveWidget->RemoveFromParent();
			}
		}
	}
}


//Update Friend list
void UListOfChatUserWidget::UpdateFriendChanges()
{
	if (p_ChatClient != nullptr)
	{
		TMap<FString, TSharedPtr<MatrixRoomSession>> FriendRoomSession = p_ChatClient->GetSession().GetFriendRooms();
		if (FriendRoomSession.Num())
		{
			for (auto Room : FriendRoomSession)
			{
				if (!FriendDisplayRoomsMap.Contains(Room.Key))
				{
					AddFriendInList(Room.Key,false);
				}
			}
		}
	}
}

//Update Friend Request List
void UListOfChatUserWidget::UpdateFriendRequestList()
{
	if (p_ChatClient != nullptr)
	{
		TArray<FMatrixInvitedRoom> InvitedRoom = p_ChatClient->GetSession().GetFriendInvites();
		if (InvitedRoom.Num())
		{
			for (auto& InviteInfo : InvitedRoom)
			{
				if (!FriendDisplayRoomsMap.Contains(InviteInfo.RoomID))
				{
					AddFriendInList(InviteInfo.RoomID, true);
				}
			}
		}
	}
}

void UListOfChatUserWidget::AddFriendInList(FString InRoomID, bool bIsRequest)
{
	FString RoomID = InRoomID;
	if (ProfileBarClass != nullptr)
	{
		UProfileBarWidget* ProfileRow = CreateWidget<UProfileBarWidget>(GetWorld(), ProfileBarClass);
		if (ProfileRow != nullptr)
		{
			ProfileRow->FriendProfileBar(RoomID,bIsRequest);
			ListScrollBox->AddChild(ProfileRow);
			FriendDisplayRoomsMap.Add(RoomID, ProfileRow);
		}
	}
}



void UListOfChatUserWidget::OnFriendRequestRecived(FMatrixInvitedRoom InRoomInfo)
{
	//bool use for Request Notify  [set as true]
	AddFriendInList(InRoomInfo.RoomID, true);
}



//Search List of World Friend
void UListOfChatUserWidget::OnNewFriendAddButtonClicked()
{
	if (SearchUserClass != nullptr)
	{
		USearchNewUserWidget* SearchUserWidget = CreateWidget<USearchNewUserWidget>(GetWorld(), SearchUserClass);
		if (SearchUserWidget != nullptr)
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController != nullptr)
			{
				ABaseHUD* HUD = Cast<ABaseHUD>(PlayerController->GetHUD());

				if (HUD != nullptr)
				{
					MenuWidget = Cast<UBaseMenuWidget>(HUD->GetCurrentMenuWidget());
					if (MenuWidget != nullptr)
					{
						MenuWidget->SetBlankSizeBox(SearchUserWidget);
					}
				}
			}
		}
	}
}

// its used for Show the Create Team Widget 
void UListOfChatUserWidget::OnCreateNewTeamButtonClicked()
{
	if (CreateTeamWidgetClass != nullptr)
	{
		UTeamCreateWidget* CreateTeamWidget = CreateWidget<UTeamCreateWidget>(GetWorld(), CreateTeamWidgetClass);
		if (CreateTeamWidget != nullptr)
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController != nullptr)
			{
				ABaseHUD* HUD = Cast<ABaseHUD>(PlayerController->GetHUD());
				if (HUD != nullptr)
				{
					MenuWidget = Cast<UBaseMenuWidget>(HUD->GetCurrentMenuWidget());
					if (MenuWidget != nullptr)
					{
						MenuWidget->SetBlankSizeBox(CreateTeamWidget);
					}
				}
			}
		}
	}
}
// its used for Search Team Functionlity
void UListOfChatUserWidget::OnSearchEditableTextBoxClicked(const FText& text)
{
	FString SearchText = text.ToLower().ToString();
	for (auto ChildWidget : ListScrollBox->GetAllChildren())
	{
		UProfileBarWidget* ListInfoWidget = Cast<UProfileBarWidget>(ChildWidget);
		if (ListInfoWidget != nullptr)
		{
			if (!SearchText.IsEmpty())
			{
				FString NameOfWidget = ListInfoWidget->UserNameTextBlock->GetText().ToString();
				if (NameOfWidget.Contains(SearchText))
				{
					ListInfoWidget->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					ListInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
			else
			{
				ListInfoWidget->SetVisibility(ESlateVisibility::Visible);
			}

		}
	}
}



//
void UListOfChatUserWidget::OnTeamAdded(FString RoomID)
{
	TSharedPtr<MatrixRoomSession> RoomSession = p_ChatClient->GetSession().GetRoomSession(RoomID);
	if (RoomSession != nullptr)
	{
		AddTeamInList(RoomSession);
	}
}

void UListOfChatUserWidget::OnTeamRemove(FString RoomID)
{
	if (TeamDisplayRoomsMap.Find(RoomID))
	{
		UProfileBarWidget* RemoveWidget = TeamDisplayRoomsMap.FindAndRemoveChecked(RoomID);
		if (RemoveWidget != nullptr)
		{
			RemoveWidget->RemoveFromParent();
		}
	}
}

void UListOfChatUserWidget::UpdateTeamChanges()
{
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		EventPublicRoom = "!irlFhoFyPGKfvLWsSe:comms.tessercon.com";
	}
	if (p_ChatClient != nullptr)
	{
		TMap<FString, TSharedPtr<MatrixRoomSession>> TeamRoomSession = p_ChatClient->GetSession().GetTeamRooms();

		if (TeamRoomSession.Num())
		{
			if (TeamRoomSession.Find(EventPublicRoom))
			{
				TeamRoomSession.FindAndRemoveChecked(EventPublicRoom);
			}
			for (auto RoomPair : TeamRoomSession)
			{
				if (!TeamDisplayRoomsMap.Contains(RoomPair.Key))
				{
					TSharedPtr<MatrixRoomSession> TeamRoomInfo = RoomPair.Value;
					FString TeamName = TeamRoomInfo->GetRoomName();
					AddTeamInList(TeamRoomInfo);
				}
			}
		}
	}
}

void UListOfChatUserWidget::AddTeamInList(TSharedPtr<MatrixRoomSession> InRoomSessionInfo)
{
	if (ProfileBarClass != nullptr && InRoomSessionInfo != nullptr)
	{
		UProfileBarWidget* ProfileRow = CreateWidget<UProfileBarWidget>(GetWorld(), ProfileBarClass);
		if (ProfileRow != nullptr)
		{
			ProfileRow->TeamProfileBar(InRoomSessionInfo);
			ListScrollBox->AddChild(ProfileRow);
			TeamDisplayRoomsMap.Add(InRoomSessionInfo->GetRoomID(), ProfileRow);
		}	
	}
}

void UListOfChatUserWidget::OnTeamRequestRecived(FMatrixInvitedRoom InRoomInfo)
{
	AddTeamRequestInList(InRoomInfo);
}

void UListOfChatUserWidget::UpdateTeamRequestList()
{
	TArray<FMatrixInvitedRoom> InvitedRoom = p_ChatClient->GetSession().GetTeamInvites();
	if (InvitedRoom.Num())
	{
		int ReversCount = InvitedRoom.Num();
		InvitedRoom.Reserve(ReversCount);
		{
			for (auto& InviteInfo : InvitedRoom)
			{
				AddTeamRequestInList(InviteInfo);
			}
		}
	}
}

void UListOfChatUserWidget::AddTeamRequestInList(FMatrixInvitedRoom InRoomInfo)
{
	if (ProfileBarClass != nullptr)
	{
		UProfileBarWidget* ProfileRow = CreateWidget<UProfileBarWidget>(GetWorld(), ProfileBarClass);
		if (ProfileRow != nullptr)
		{
		    ProfileRow->TeamRequestProfileBar(InRoomInfo);
			ListScrollBox->AddChild(ProfileRow);
			TeamDisplayRoomsMap.Add(InRoomInfo.RoomID, ProfileRow);
		}
	}
}

void UListOfChatUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
	FriendDisplayRoomsMap.Empty();
}