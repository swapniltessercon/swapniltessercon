// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BaseMenuWidget.h"

#include "UObject/ConstructorHelpers.h"
//Classes
#include "UI/MainMenu/worldMessageBodyWidget.h"
#include "UI/MainMenu/ListOfChatUserWidget.h"
#include "UI/MainMenu/BoothListWidget.h"
#include "UI/SelectEventMenu/SelectEventMenuWidget.h"
#include "UI/Settings/SettingsMenuWidget.h"
#include "UI/MainMenu/CallInfoListWidget.h"
#include "UI/MainMenu/CallWidget.h"

//Downloading
#include "Blueprint/AsyncTaskDownloadImage.h"
//Components
#include "UI/TesserconButton.h"
#include "Components/SizeBox.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

UBaseMenuWidget::UBaseMenuWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> SettingsBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/Settings_WBP"));
	if (SettingsBPObj.Class != nullptr)
	{
		SettingsClass = SettingsBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> UserProfileBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/UserProfile_WBP"));
	if (UserProfileBPObj.Class != nullptr)
	{
		UserProfileClass = UserProfileBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> RunningChatMessageBodyBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/WorldMessageBody_WBP"));
	if (RunningChatMessageBodyBPObj.Class != nullptr)
	{
		RunningChatMessageBodyClass = RunningChatMessageBodyBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> ListOfChatUserBodyBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/ListOfChatUser_WBP"));
	if (ListOfChatUserBodyBPObj.Class != nullptr)
	{
		ListOfChatUserBodyClass = ListOfChatUserBodyBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> ProfileBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/ProfileBar_WBP"));
	if (ProfileBPObj.Class != nullptr)
	{
		ProfileBarClass = ProfileBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> CallInfoListBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/CallList_WBP"));
	if (CallInfoListBPObj.Class != nullptr)
	{
		CallInfoListClass = CallInfoListBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> OnCallWidgetBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/OnCallWidget_WBP"));
	if (OnCallWidgetBPObj.Class != nullptr)
	{
		OnCallWidgetclass = OnCallWidgetBPObj.Class;
	}
}

void UBaseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SidePanelChatButton != nullptr)
	{
		SidePanelChatButton->OnClicked.AddDynamic(this, &UBaseMenuWidget::OnSidePanelChatButtonClicked);
	}

	if (SidePanelGroupsButton != nullptr)
	{
		SidePanelGroupsButton->OnClicked.AddDynamic(this, &UBaseMenuWidget::OnSidePanelGroupsButtonClicked);
	}

	if (SidePanelCallButton != nullptr)
	{
		SidePanelCallButton->OnClicked.AddDynamic(this, &UBaseMenuWidget::OnSidePanelCallButtonClicked);
	}

	if (SidePanelActivityButton != nullptr)
	{
		SidePanelActivityButton->OnClicked.AddDynamic(this, &UBaseMenuWidget::OnSidePanelActivityButtonClicked);
	}

	if (SidePanelHelpButton != nullptr)
	{
		SidePanelHelpButton->OnClicked.AddDynamic(this, &UBaseMenuWidget::OnSidePanelHelpButtonClicked);
	}

	if (SidePanelSettingsButton != nullptr)
	{
		SidePanelSettingsButton->OnClicked.AddDynamic(this, &UBaseMenuWidget::SidePanelSettingsButtonClicked);
	}

	if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
	{
		p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
		if (p_ChatClient != nullptr)
		{
			//Team Request Recived
			p_ChatClient->GetSession().d_EventOnTeamRequestReceived.AddUObject(this, &UBaseMenuWidget::OnTeamRequestRecived);
			//Friend Request Recived 
			p_ChatClient->GetSession().d_EventOnFriendRequestReceived.AddUObject(this, &UBaseMenuWidget::OnChatRequestRecived);
			//Frined Chat notification 
			p_ChatClient->GetSession().d_EventOnFriendChatEventNotification.AddUObject(this, &UBaseMenuWidget::OnFriendChatNotification);
			//Team Chat notification 
			p_ChatClient->GetSession().d_EventOnTeamChatEventNotification.AddUObject(this, &UBaseMenuWidget::OnTeamChatNotification);
		}
	}

	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		TessClient->Getsession().d_EventOnMatrixCallEventProcessed.AddUObject(this, &UBaseMenuWidget::OnResponseCallEvent);
	    TessClient->Getsession().d_EventOnCallStateChanged.AddUObject(this, &UBaseMenuWidget::OnResponseCallstate);
		TessClient->Getsession().d_EventOnMicAvailablityChanged.AddUObject(this, &UBaseMenuWidget::OnMicAvailablityChanged);
		TessClient->Getsession().d_EventOnSpeakerAvailablityChanged.AddUObject(this, &UBaseMenuWidget::OnSpeakerAvailablityChanged);
	}

	

	//Default Notification
	ShowNotificationImage();
}



void UBaseMenuWidget::NativeDestruct()
{
	//TODO
	Super::NativeDestruct();
}

FReply UBaseMenuWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!InMouseEvent.GetEventPath()->ContainsWidget(ListWidgetSizeBox->TakeWidget()))
	{
		if (SidePanelChatButton->p_SelectedButton != nullptr)
		{
			SidePanelChatButton->p_SelectedButton->SetNormal();
			SidePanelChatButton->p_SelectedButton = nullptr;
			ListWidgetSizeBox->ClearChildren();
			ChatWidgetSizeBox->ClearChildren();
		}
	}
	if(MenuBackgroundImage != nullptr)
		MenuBackgroundImage->SetVisibility(ESlateVisibility::Hidden);

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UBaseMenuWidget::ShowChatListSizeBox()
{
	ListWidgetSizeBox->SetVisibility(ESlateVisibility::Visible);
	ChatWidgetSizeBox->ClearChildren();
	if (ChatWidgetSizeBox != nullptr)
	{
		ChatWidgetSizeBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBaseMenuWidget::SetChatWidgetSizeBox(UUserWidget* ProfileWidget)
{
	ChatWidgetSizeBox->ClearChildren();
	ChatWidgetSizeBox->AddChild(ProfileWidget);
	if (ListWidgetSizeBox != nullptr)
	{
		ListWidgetSizeBox->ClearChildren();
	}
	if (BlankSizeBox->IsVisible())
	{
		BlankSizeBox->ClearChildren();
	}
}

void UBaseMenuWidget::SetBlankSizeBox(UUserWidget* ProfileWidget)
{
	BlankSizeBox->ClearChildren();
	BlankSizeBox->AddChild(ProfileWidget);
}

void UBaseMenuWidget::GetSettingsClass(TSubclassOf<UUserWidget>& InClass)
{
	InClass = SettingsClass;
}

void UBaseMenuWidget::GetUserProfileClass(TSubclassOf<UUserWidget>& InClass)
{
	InClass = UserProfileClass;
}

void UBaseMenuWidget::GetRunningChatMessageBodyClass(TSubclassOf<UUserWidget>& InClass)
{
	InClass = RunningChatMessageBodyClass;
}

void UBaseMenuWidget::GetListOfChatUserBodyClass(TSubclassOf<UUserWidget>& InClass)
{
	InClass = ListOfChatUserBodyClass;
}


void UBaseMenuWidget::GetProfileBarClass(TSubclassOf<UUserWidget>& InClass)
{
	InClass = ProfileBarClass;
}

void UBaseMenuWidget::OnSidePanelChatButtonClicked()
{
	if (SidePanelChatButton->p_SelectedButton != nullptr)
	{
		if (SidePanelChatButton->p_SelectedButton == SidePanelChatButton)
		{
			SidePanelChatButton->SetNormal();
			SidePanelChatButton->p_SelectedButton = nullptr;
			ListWidgetSizeBox->ClearChildren();
			ChatWidgetSizeBox->ClearChildren();
			if (MenuBackgroundImage != nullptr)
				MenuBackgroundImage->SetVisibility(ESlateVisibility::Hidden);
			return;
		}
		else
		{
			SidePanelChatButton->p_SelectedButton->SetNormal();
		}
	}
	SidePanelChatButton->SetSelected();
	SidePanelChatButton->p_SelectedButton = SidePanelChatButton;
	if (MenuBackgroundImage != nullptr)
		MenuBackgroundImage->SetVisibility(ESlateVisibility::Visible);
	ShowChatMenu();
}
void UBaseMenuWidget::ShowChatMenu()
{
	ListWidgetSizeBox->ClearChildren();
	ChatWidgetSizeBox->ClearChildren();
	UListOfChatUserWidget* ChatList;
	if (ListOfChatUserBodyClass != nullptr)
	{
		ChatList = CreateWidget<UListOfChatUserWidget>(GetWorld(), ListOfChatUserBodyClass);
		if (ChatList != nullptr)
		{
			ChatList->Setup(EChatType::Single);
			ListWidgetSizeBox->AddChild(ChatList);
		}
	}
}

void UBaseMenuWidget::OnSidePanelGroupsButtonClicked()
{
	if (SidePanelGroupsButton->p_SelectedButton != nullptr)
	{
		if (SidePanelGroupsButton->p_SelectedButton == SidePanelGroupsButton)
		{
			SidePanelGroupsButton->SetNormal();
			SidePanelGroupsButton->p_SelectedButton = nullptr;
			ListWidgetSizeBox->ClearChildren();
			ChatWidgetSizeBox->ClearChildren();
			if (MenuBackgroundImage != nullptr)
				MenuBackgroundImage->SetVisibility(ESlateVisibility::Hidden);
			return;
		}
		else
		{
			SidePanelGroupsButton->p_SelectedButton->SetNormal();
		}
	}
	SidePanelGroupsButton->SetSelected();
	SidePanelGroupsButton->p_SelectedButton = SidePanelGroupsButton;

	if (MenuBackgroundImage != nullptr)
		MenuBackgroundImage->SetVisibility(ESlateVisibility::Visible);

	ShowGroup();
}

void UBaseMenuWidget::ShowGroup()
{
	ListWidgetSizeBox->ClearChildren();
	ChatWidgetSizeBox->ClearChildren();
	UListOfChatUserWidget* ChatList;
	if (ListOfChatUserBodyClass != nullptr)
	{
		ChatList = CreateWidget<UListOfChatUserWidget>(GetWorld(), ListOfChatUserBodyClass);
		if (ChatList != nullptr)
		{
			ChatList->Setup(EChatType::Group);
			ListWidgetSizeBox->AddChild(ChatList);
		}
	}
}

void UBaseMenuWidget::OnSidePanelCallButtonClicked()
{
	if (SidePanelCallButton->p_SelectedButton != nullptr)
	{
		if (SidePanelCallButton->p_SelectedButton == SidePanelCallButton)
		{
			SidePanelCallButton->SetNormal();
			SidePanelCallButton->p_SelectedButton = nullptr;
			CallLogSizeBox->SetVisibility(ESlateVisibility::Collapsed);
			if (MenuBackgroundImage != nullptr)
				MenuBackgroundImage->SetVisibility(ESlateVisibility::Hidden);
			return;
		}
		else
		{
			SidePanelCallButton->p_SelectedButton->SetNormal();
		}
	}
	SidePanelCallButton->SetSelected();
	SidePanelCallButton->p_SelectedButton = SidePanelCallButton;
	if (CallLogSizeBox->IsVisible())
	{
		CallLogSizeBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		CallLogSizeBox->SetVisibility(ESlateVisibility::Visible);
	}
}

void UBaseMenuWidget::OnSidePanelActivityButtonClicked()
{
	if (SidePanelActivityButton->p_SelectedButton != nullptr)
	{
		if (SidePanelActivityButton->p_SelectedButton == SidePanelActivityButton)
		{
			SidePanelActivityButton->SetNormal();
			SidePanelActivityButton->p_SelectedButton = nullptr;
			if (MenuBackgroundImage != nullptr)
				MenuBackgroundImage->SetVisibility(ESlateVisibility::Hidden);
			return;
		}
		else
		{
			SidePanelActivityButton->p_SelectedButton->SetNormal();
		}
	}
	SidePanelActivityButton->SetSelected();
	SidePanelActivityButton->p_SelectedButton = SidePanelActivityButton;

	if (MenuBackgroundImage != nullptr)
		MenuBackgroundImage->SetVisibility(ESlateVisibility::Visible);
}

void UBaseMenuWidget::OnSidePanelHelpButtonClicked()
{
	//TODO
}

void UBaseMenuWidget::SidePanelSettingsButtonClicked()
{
	if (SettingsClass != nullptr)
	{
		USettingsMenuWidget* SettingsWidget = CreateWidget<USettingsMenuWidget>(GetWorld(), SettingsClass);
		if (SettingsWidget != nullptr)
		{
			SetBlankSizeBox(SettingsWidget);
		}
	}
}

void UBaseMenuWidget::OnTeamRequestRecived(FMatrixInvitedRoom InRoomInfo)
{
	GroupNotifyImage->SetVisibility(ESlateVisibility::Visible);
}

void UBaseMenuWidget::ShowNotificationImage()
{
	//Create A call Widget 
	if (CallInfoListClass != nullptr)
	{
		CallNotifyWidget = CreateWidget<UCallInfoListWidget>(GetWorld(), CallInfoListClass);
		CallLogSizeBox->AddChild(CallNotifyWidget);
		CallLogSizeBox->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (p_ChatClient != nullptr)
	{
		bool bIsFrinedChatNotify = p_ChatClient->GetSession().HasFriendChatNewNotifications();
		bool bIsTeamChatNotify = p_ChatClient->GetSession().HasTeamChatNewNotifications();
		if (bIsFrinedChatNotify)
		{
			ChatNotifyImage->SetVisibility(ESlateVisibility::Visible);
		}
		if (bIsTeamChatNotify)
		{
			GroupNotifyImage->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UBaseMenuWidget::OnChatRequestRecived(FMatrixInvitedRoom InRoomInfo)
{
	GroupNotifyImage->SetVisibility(ESlateVisibility::Visible);
}

void UBaseMenuWidget::OnTeamChatNotification(bool bIsCheckNotify)
{
	if (bIsCheckNotify)
	{
		GroupNotifyImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		GroupNotifyImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBaseMenuWidget::OnFriendChatNotification(bool bIsCheckNotify)
{
	if (bIsCheckNotify)
	{
		ChatNotifyImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ChatNotifyImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBaseMenuWidget::OnResponseCallEvent(EMatrixCallEventType EventType, FString RoomID, FMatrixCallEvent CalledInfo)
{
	if (EventType == EMatrixCallEventType::Invite && CallNotifyWidget != nullptr)
	{
		CallNotifyWidget->CallNotifinctionWindowSetup(EventType, RoomID, CalledInfo);
		CallLogSizeBox->SetVisibility(ESlateVisibility::Visible);
	}
}

void UBaseMenuWidget::OnResponseCallstate(FTesserconCallState CallStateInfo)
{
	if (CallBarSizeBox != nullptr)
	{
		UCallWidget* ExistingOngoingCallWidget = Cast<UCallWidget>(CallBarSizeBox->GetChildAt(0));
		bool bDoesMatchWithExistingCall = false;
		if (ExistingOngoingCallWidget != nullptr)
		{
			if (ExistingOngoingCallWidget->GetCallID() == CallStateInfo.CallID)
			{
				bDoesMatchWithExistingCall = true;
				UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::OnResponseCallstate :: bDoesMatchWithExistingCall"));
			}
		}
		if (!bDoesMatchWithExistingCall)
		{
			if (CallStateInfo.CallStatus == ETesserconCallStatus::Dialing || CallStateInfo.CallStatus == ETesserconCallStatus::Connecting/*OnCall /*|| CallStateInfo.CallStatus == ETesserconCallStatus::SettingupTransition*/)
			{
				if (OnCallWidgetclass != nullptr)
				{
					UCallWidget* OngoingCallWidget = CreateWidget<UCallWidget>(GetWorld(), OnCallWidgetclass);
					if (CallBarSizeBox != nullptr && OngoingCallWidget != nullptr)
					{
						CallBarSizeBox->ClearChildren();
						OngoingCallWidget->Setup(CallStateInfo);
						CallBarSizeBox->AddChild(OngoingCallWidget);
						UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::OnResponseCallstate ::CallWindowSizeBox->AddChild(OngoingCallWidget)"));
						if (CallStateInfo.CallStatus == ETesserconCallStatus::OnCall && CallLogSizeBox != nullptr)
						{
							CallLogSizeBox->SetVisibility(ESlateVisibility::Collapsed);
						}
					}
				}
			}
		}
	}
}

void UBaseMenuWidget::OnMicAvailablityChanged(bool bIsMicAvilabel)
{
	if (bIsMicAvilabel)
	{
		if (CallNotifyWidget != nullptr)
		{
			CallNotifyWidget->HideInforamtionBox();
			CallLogSizeBox->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		if (CallNotifyWidget != nullptr)
		{
			CallNotifyWidget->ShowMicInforamtion();
			CallLogSizeBox->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UBaseMenuWidget::OnSpeakerAvailablityChanged(bool bIsSpeakerAvilabel)
{
	if (bIsSpeakerAvilabel)
	{
		if (CallNotifyWidget != nullptr)
		{
			CallNotifyWidget->HideInforamtionBox();
			CallLogSizeBox->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		if (CallNotifyWidget != nullptr)
		{
			CallNotifyWidget->ShowMicInforamtion();
			CallLogSizeBox->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UBaseMenuWidget::HideCallNotification()
{
	CallLogSizeBox->SetVisibility(ESlateVisibility::Collapsed);
}



void UBaseMenuWidget::SetCallBarSizeBox(UUserWidget* ProfileWidget)
{
	CallBarSizeBox->ClearChildren();
	CallBarSizeBox->AddChild(ProfileWidget);

}