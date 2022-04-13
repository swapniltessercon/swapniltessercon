// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/SearchNewUserWidget.h"

//class
#include "UI/MainMenu/TeamInfoProfileBarWidget.h"

//Components
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"



USearchNewUserWidget::USearchNewUserWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> TeamUserProfileBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/TeamListProfileBar_wBP"));
	if (TeamUserProfileBPObj.Class != nullptr)
	{
		TeamUserProfileClass = TeamUserProfileBPObj.Class;
	}
}

void USearchNewUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SearchEditableTextBox != nullptr)
	{
		SearchEditableTextBox->OnTextChanged.AddDynamic(this, &USearchNewUserWidget::OnSearchEditableTextBoxClicked);
	}
	if (SearchButton != nullptr)
	{
		SearchButton->OnClicked.AddDynamic(this, &USearchNewUserWidget::OnSearchButtonClicked);
	}
	if (CloseButton != nullptr)
	{
		CloseButton->OnClicked.AddDynamic(this, &USearchNewUserWidget::RemoveFromParent);
	}
	if (BGImage != nullptr)
	{
		BGImage->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDownImage"));
	}

	GetAllEventMembers();
}

void USearchNewUserWidget::OnSearchEditableTextBoxClicked(const FText& text)
{
	FString SearchText = text.ToLower().ToString();


	if (SearchText.IsEmpty())
	{
		for (auto Info : ListOfWorldMemberMap)
		{
			if (Info.Value->IsVisible())
			{
				Info.Value->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}


void  USearchNewUserWidget::GetAllEventMembers()
{
	if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
	{     //TODO :: Get Public Room ID
		FString EventPublicRoom = "!irlFhoFyPGKfvLWsSe:comms.tessercon.com";
		p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
		TSharedPtr<MatrixRoomSession> RoomSession = p_ChatClient->GetSession().GetRoomSession(EventPublicRoom);

		if (RoomSession != nullptr)
		{
			TArray<FString> TeamMembersID = RoomSession->GetMemberListForMembership(EMatrixMembership::Join);
			if (TeamMembersID.Num())
			{
				for (auto Member : TeamMembersID)
				{
					AddEventParticipant(Member);

				}
			}
		}
	}
}

void USearchNewUserWidget::OnSearchButtonClicked()
{
	FString SearchName = SearchEditableTextBox->GetText().ToString();
	SearchName = SearchName.ToLower();
	for (auto Info : ListOfWorldMemberMap)
	{
		if (Info.Key.ToLower() == SearchName)
		{
			SearchUserScrollBox->AddChild(Info.Value);
		} 
	

	}
}

void USearchNewUserWidget::AddEventParticipant(FString UserID)
{
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		FString LoggedUsername = TessClient->Getsession().GetLoggedInUserInfo().Username;
		FString Username = GetUsernameFromMatrixUsername(UserID);
		if (LoggedUsername != Username)
		{
			TesserconAPI::FOnUserInfoByUserIDResponseDelegate  RequestListCallback;
			RequestListCallback.BindLambda([this](ETesserconError Error, FTesserconUserInfo UserInfo)
				{
					if (Error == ETesserconError::Success)
					{
						if (TeamUserProfileClass == nullptr)return;
					    ProfileRow = CreateWidget<UTeamInfoProfileBarWidget>(GetWorld(), TeamUserProfileClass);
						if (ProfileRow == nullptr) return;
						ProfileRow->SetSearchMemberProfile(UserInfo);
						FString Name = UserInfo.FirstName + " " +UserInfo.LastName;
						ListOfWorldMemberMap.Add(Name, ProfileRow);
					}
				});
			TessClient->Getsession().GetUserInfoByUsername(Username, RequestListCallback);
			
		}
	}
}

void USearchNewUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

FEventReply USearchNewUserWidget::OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent)
{
	RemoveFromParent();
	return FEventReply(false);
}