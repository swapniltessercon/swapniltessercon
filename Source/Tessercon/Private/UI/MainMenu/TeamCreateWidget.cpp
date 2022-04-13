// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/TeamCreateWidget.h"

//class
#include "UI/MainMenu/TeamInfoProfileBarWidget.h"

//Module
#include "TesserconCore.h"
#include "MatrixCore.h"

//Components
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

UTeamCreateWidget::UTeamCreateWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> TeamUserProfileBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/TeamListProfileBar_wBP"));
	if (TeamUserProfileBPObj.Class != nullptr)
	{
		TeamUserProfileClass = TeamUserProfileBPObj.Class;
	}
}

void UTeamCreateWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SearchEditableTextBox != nullptr)
	{
		SearchEditableTextBox->OnTextChanged.AddDynamic(this, &UTeamCreateWidget::OnSearchEditableTextBoxChanged);
	}
	if (TeamNameEditableTextBox != nullptr)
	{
		TeamNameEditableTextBox->OnTextChanged.AddDynamic(this, &UTeamCreateWidget::OnTeamNameEditableTextBoxChanged);
	}
	if (ChangeGroupIconButton != nullptr)
	{
		ChangeGroupIconButton->OnClicked.AddDynamic(this, &UTeamCreateWidget::OnChangeGroupIconButtonClicked);
	}
	if (CancelButton != nullptr)
	{
		CancelButton->OnClicked.AddDynamic(this, &UTeamCreateWidget::RemoveFromParent);
	}
	if (NextButton != nullptr)
	{
		NextButton->OnClicked.AddDynamic(this, &UTeamCreateWidget::OnNextButtonClicked);
	}
	if (TeamCreateButton != nullptr)
	{
		TeamCreateButton->OnClicked.AddDynamic(this, &UTeamCreateWidget::OnTeamCreateButtonClicked);
	}
	if (TeamCreateCancelButton != nullptr)
	{
		TeamCreateCancelButton->OnClicked.AddDynamic(this, &UTeamCreateWidget::RemoveFromParent);
	}

	ListSizeBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UTeamCreateWidget::OnSearchEditableTextBoxChanged(const FText& text)
{
	FString SearchText = text.ToLower().ToString();
	for (auto ChildWidget : SearchListScrollBox->GetAllChildren())
	{
		UTeamInfoProfileBarWidget* ListInfoWidget = Cast<UTeamInfoProfileBarWidget>(ChildWidget);
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

void UTeamCreateWidget::OnTeamNameEditableTextBoxChanged(const FText& text)
{
	FText Text = ClampText(text, 20);
	TeamNameEditableTextBox->SetText(FText(Text));
}

FText UTeamCreateWidget::ClampText(FText Text, int32 MaxCount)
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

void UTeamCreateWidget::OnNextButtonClicked()
{
	FString TeamName =TeamNameEditableTextBox->GetText().ToString();
	if (!TeamName.IsEmpty())
	{
		AvatarHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
		ListSizeBox->SetVisibility(ESlateVisibility::Visible);
		if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
		{
			p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
		}
		if (p_ChatClient != nullptr)
		{
			TMap<FString, TSharedPtr<MatrixRoomSession>> FriendRoomSession = p_ChatClient->GetSession().GetFriendRooms();
			for (auto RoomPair : FriendRoomSession)
			{
				FString UserID;
				UserID = p_ChatClient->GetSession().GetFriendUserIDForRoom(RoomPair.Key);
				if (!UserID.IsEmpty())
				{
					FString Username = GetUsernameFromMatrixUsername(UserID);
					TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
					if (TessClient != nullptr)
					{
						TesserconAPI::FOnUserInfoByUserIDResponseDelegate  RequestListCallback;
						RequestListCallback.BindLambda([this](ETesserconError Error, FTesserconUserInfo UserInfo)
							{
								if (Error == ETesserconError::Success)
								{
									if (TeamUserProfileClass == nullptr)return;
									UTeamInfoProfileBarWidget* ProfileRow = CreateWidget<UTeamInfoProfileBarWidget>(GetWorld(), TeamUserProfileClass);
									if (ProfileRow == nullptr) return;
									ProfileRow->SetProfile(this, UserInfo);
									SearchListScrollBox->SetScrollOffset(0);
									SearchListScrollBox->AddChild(ProfileRow);
								}
							});
						TessClient->Getsession().GetUserInfoByUsername(Username, RequestListCallback);
					}

				}
			}
		}
	}
}

void UTeamCreateWidget::OnChangeGroupIconButtonClicked() 
{

}

void UTeamCreateWidget::OnTeamCreateButtonClicked()
{
	FString TeamName = TeamNameEditableTextBox->GetText().ToString();
	
	TArray<FString> TeamMemberID;
	for (auto& TeamUserInfo : MemberInfo)
	{
		FString UserID = GetMatrixFullUserIDForUsername(TeamUserInfo.Key);
		TeamMemberID.Add(UserID);
	}
	if (p_ChatClient != nullptr)
	{
		MatrixNativeAPI::FOnMatrixCreateRoomReponseDelegate CreateTeamRoomCallBack;
		CreateTeamRoomCallBack.BindLambda([this](EMatrixCoreError Error, FString UserId)
			{
				if (Error == EMatrixCoreError::Success)
				{
					MemberInfo.Empty();
					TeamNameEditableTextBox->SetText(FText::FromString(""));
					RemoveFromParent();
				}

			});
		p_ChatClient->GetSession().CreateRoom(TeamMemberID, false, TeamName, CreateTeamRoomCallBack);
	}
	
}

void UTeamCreateWidget::AddInTeamList(FTesserconUserInfo InUserInfo)
{
	FString TeamMemberKey = InUserInfo.Username;
	int Flag = 0;
	if (MemberInfo.Num() == 0)
	{
		MemberInfo.Add(TeamMemberKey, InUserInfo);
	}
	else
	{
		for (auto& TeamUserInfo : MemberInfo)
		{
			if (TeamUserInfo.Key == TeamMemberKey)
			{
				Flag++;
				break;
			}
		}
		if (Flag > 0)
		{
			MemberInfo.Remove(TeamMemberKey);
		}
		else
		{
			MemberInfo.Add(TeamMemberKey, InUserInfo);
		}
	}
}



//void UTeamCreateWidget::OnTeamCreateCancelButtonClicked() {}
//void UTeamCreateWidget::OnCancelButtonClicked()	{}