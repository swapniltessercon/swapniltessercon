// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/MainMenuWidget.h"
//class
#include "UI/SelectEventMenu/SelectEventMenuWidget.h"
#include "UI/MainMenu/BoothListWidget.h"
#include "UI/Profile/UserProfileWidget.h"
#include "UI/MainMenu/worldMessageBodyWidget.h"

#include "UI/MainMenu/AvatarSelectorWidget.h"

//GameInstance
#include "TesserconGameInstance.h"
//DownLoading 
#include "Blueprint/AsyncTaskDownloadImage.h"

//components
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"


UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//TODO
	static ConstructorHelpers::FClassFinder<UUserWidget> SelectEventMenuBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/SelectEvent/SelectEventMenu_WBP"));
	if (SelectEventMenuBPObj.Class != nullptr)
	{
		SelectEventClass = SelectEventMenuBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> BoothListBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/BoothList_WBP"));
	if (BoothListBPObj.Class != nullptr)
	{
		BoothListClass = BoothListBPObj.Class;
	}


	static ConstructorHelpers::FClassFinder<UUserWidget> AvatarSelectorBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/AvatarSelector_WBP"));
	//static ConstructorHelpers::FClassFinder<UUserWidget> AvatarSelectorBPObj(TEXT("/Game/CharacterCustomizer/Character_Customizer/UMG/Widgets/CC_Apparel_Menu"));
	if (AvatarSelectorBPObj.Class != nullptr)
	{
		AvatarSelectorClass = AvatarSelectorBPObj.Class;
	}
}

void UMainMenuWidget::NativeConstruct()
{
	//TODO
	Super::NativeConstruct();

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			m_LoggedInUserInfo = TessClient->Getsession().GetLoggedInUserInfo();
			if (m_LoggedInUserInfo.UserRole == EUserRole::Attendee)
			{
				EnterBoothButton->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	SetUserProfile();


	p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
	if (p_ChatClient != nullptr)
	{
		p_ChatClient->GetSession().d_EventOnTeamAdded.AddUObject(this, &UMainMenuWidget::OnTeamAddedResponseReceived);

	}
	if (EventBannerImage != nullptr)
	{
		UAsyncTaskDownloadImage* UrlImageTask = NewObject<UAsyncTaskDownloadImage>();
		UrlImageTask->OnSuccess.AddDynamic(this, &UMainMenuWidget::GetEventImage);

		UTesserconGameInstance* GameInstance = static_cast<UTesserconGameInstance*>(GetGameInstance());

		UrlImageTask->Start(GameInstance->GetEvent().EventBannerURL);
	}

	if (ConferenceButton != nullptr)
	{
		ConferenceButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnConferenceButtonClicked);
	}

	if (BoothListButton != nullptr)
	{
		BoothListButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBoothListButtonClicked);
	}

	if (UserProfileButton != nullptr)
	{
		UserProfileButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnUserProfileButtonClicked);
	}

	if (EnterEventButton != nullptr)
	{
		EnterEventButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnEnterEventButtonClicked);
	}

	if (MessageEditableTextBox != nullptr)
	{
		MessageEditableTextBox->OnTextCommitted.AddDynamic(this, &UMainMenuWidget::OnMessageContentCommitted);
	}
	if (AvatarButton != nullptr)
	{
		AvatarButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnAvatarButtonClicked);
	}

	if (BackButton != nullptr)
	{
		BackButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBackButtonClicked);
	}


	
	if (MenuWidgetSwitcher != nullptr)
	{
		MenuWidgetSwitcher->SetActiveWidgetIndex(0);
	}
	
}



void UMainMenuWidget::OnAvatarButtonClicked() 
{
	if (MenuWidgetSwitcher != nullptr)
	{
		MenuWidgetSwitcher->SetActiveWidgetIndex(1);

		 SelectAvatarWidget = CreateWidget<UAvatarSelectorWidget>(GetWorld(), AvatarSelectorClass);
		if (SelectAvatarWidget != nullptr)
		{
			SelectAvatarWidget->AddToViewport();
		}
	}
}


void UMainMenuWidget::OnBackButtonClicked()
{
	if (MenuWidgetSwitcher != nullptr)
	{
		MenuWidgetSwitcher->SetActiveWidgetIndex(0);
		SelectAvatarWidget->RemoveFromParent();
	}
}

void UMainMenuWidget::NativeDestruct()
{
	//TODO
	Super::NativeDestruct();
}


void UMainMenuWidget::OnConferenceButtonClicked()
{
	if (SelectEventClass != nullptr)
	{
		USelectEventMenuWidget* SelectEventWidget = CreateWidget<USelectEventMenuWidget>(GetWorld(), SelectEventClass);
		if (SelectEventWidget != nullptr)
		{
			SetBlankSizeBox(SelectEventWidget);
		}
	}
}

void UMainMenuWidget::OnUserProfileButtonClicked()
{
	if (UserProfileClass != nullptr)
	{
		UUserProfileWidget* UserProfileWidget = CreateWidget<UUserProfileWidget>(GetWorld(), UserProfileClass);
		if (UserProfileWidget != nullptr)
		{
			UserProfileWidget->SetProfile(m_LoggedInUserInfo);
			SetBlankSizeBox(UserProfileWidget);
		}
	}
}

void UMainMenuWidget::OnBoothListButtonClicked()
{
	if (BoothListClass != nullptr)
	{
		UBoothListWidget* BoothListWidget = CreateWidget<UBoothListWidget>(GetWorld(), BoothListClass);
		if (BoothListWidget != nullptr)
		{
			BlankSizeBox->AddChild(BoothListWidget);
		}
	}
}

void UMainMenuWidget::GetEventImage(UTexture2DDynamic* Texture)
{
	EventBannerImage->SetBrushFromTextureDynamic(Texture);
}

void UMainMenuWidget::OnEnterBoothButtonClicked()
{
	//TODO
}

void UMainMenuWidget::OnEnterEventButtonClicked()
{
	/*UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FString TravelURL = "/Game/BaseCore/Maps/InGame/InGameLevel";
	PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);*/


	FString TravelURL = "/Game/BaseCore/Maps/InGame/InGameLevel";


	FString Level = TravelURL + "?Listen";
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	World->ServerTravel(Level);

}

void UMainMenuWidget::SetUserProfile()
{
	FString Name = m_LoggedInUserInfo.FirstName + " " + m_LoggedInUserInfo.LastName;
	UserNameTextBlock->SetText(FText::FromString(Name));
	DesignationTextBlock->SetText(FText::FromString(m_LoggedInUserInfo.Designation));
	if (UserProfileImage != nullptr)
	{
		UAsyncTaskDownloadImage* DownloadedImageTask = NewObject<UAsyncTaskDownloadImage>();
		DownloadedImageTask->OnSuccess.AddDynamic(this, &UMainMenuWidget::OnSetProfileTexture);
		DownloadedImageTask->Start(m_LoggedInUserInfo.ProfileImageURL);
	}
}

void UMainMenuWidget::OnSetProfileTexture(UTexture2DDynamic* Texture)
{
	if(UserProfileImage == nullptr) return;
	UserProfileImage->SetBrushFromTextureDynamic(Texture);
}



void UMainMenuWidget::OnTeamAddedResponseReceived(FString RoomID)
{

	FString EventPublicRoom = "!irlFhoFyPGKfvLWsSe:comms.tessercon.com";
	if (RoomID == EventPublicRoom)
	{
		if (p_ChatClient != nullptr)
		{
			m_ChatRoomSession = p_ChatClient->GetSession().GetRoomSession(EventPublicRoom);
		}
		if (m_ChatRoomSession != nullptr)
		{
			m_ChatRoomSession->d_EventOnNewMessagesReceived.AddUObject(this, &UMainMenuWidget::OnMessageResponseReceived);
		}
	}

}

void UMainMenuWidget::OnMessageResponseReceived(FMatrixMesssageEvents RoomMessageEvent)
{
	for (auto Pair : RoomMessageEvent.EventOrder)
	{
		if (Pair.Key == EJoinedRoomEventType::Message)
		{
			if (RunningChatMessageBodyClass != nullptr)
			{
				UworldMessageBodyWidget* MessageBodyWidget = CreateWidget<UworldMessageBodyWidget>(GetWorld(), RunningChatMessageBodyClass);
				if (MessageBodyWidget != nullptr)
				{

					FMatrixTextMessage Message = RoomMessageEvent.Messages[Pair.Value];
					MessageBodyWidget->SendMessageInfo(Message.Message, Message.Sender);
					RunningChatScrollBox->AddChild(MessageBodyWidget);
					RunningChatScrollBox->ScrollToEnd();
					UE_LOG(LogTemp, Warning, TEXT("Text %s : %s"), *Message.Message, *Message.Sender);
				}
			}
		}
	}

}


void UMainMenuWidget::OnMessageContentCommitted(const FText& InText, ETextCommit::Type InCommitType)
{
	FString Message = MessageEditableTextBox->GetText().ToString();
	if (InCommitType == ETextCommit::Type::OnEnter)
	{
		if (m_ChatRoomSession != nullptr)
		{
			MatrixNativeAPI::FOnMatrixAPIResponseDelegate MessageResponceCallBack;
			MessageResponceCallBack.BindLambda([this, Message](EMatrixCoreError Error) {

				if (Error == EMatrixCoreError::Success)
				{
					MessageEditableTextBox->SetText(FText::FromString(""));
				}
				});
			m_ChatRoomSession->SendMessage(Message, MessageResponceCallBack);
		}
	}
}