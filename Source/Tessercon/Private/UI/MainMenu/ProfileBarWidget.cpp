// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/ProfileBarWidget.h"

//class
#include "UI/MainMenu/ChatWindowWidget.h"
//Download
#include "Blueprint/AsyncTaskDownloadImage.h"
//Components
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"


UProfileBarWidget::UProfileBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> UserChatWindowBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/UserChat_WBP"));
	if (UserChatWindowBPObj.Class != nullptr)
	{
		UserChatWindowClass = UserChatWindowBPObj.Class;
	}
}

void UProfileBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ProfileButton != nullptr)
	{
		ProfileButton->OnClicked.AddDynamic(this, &UProfileBarWidget::OnProfileButtonClicked);
	}
}

void UProfileBarWidget::FriendProfileBar(FString InRoomID, bool bIsRequest)
{
	 bIsCheckRequest = bIsRequest;
     RoomID = InRoomID;
	 bIsCheckGroup = false;

		if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
		{
			p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();

			if (p_ChatClient != nullptr)
			{
				FString FriendUserID = p_ChatClient->GetSession().GetFriendUserIDForRoom(RoomID);
				FString Username = GetUsernameFromMatrixUsername(FriendUserID);
				TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
				if (TessClient != nullptr)
				{
					TesserconAPI::FOnUserInfoByUserIDResponseDelegate  RequestListCallback;
					RequestListCallback.BindLambda([this](ETesserconError Error, FTesserconUserInfo InUserInfo)
						{
							if (Error == ETesserconError::Success)
							{
								UserInfo = InUserInfo;
								SetProfileInfo(UserInfo);
							}
						});
					TessClient->Getsession().GetUserInfoByUsername(Username, RequestListCallback);
				}

				ChatRoomSession = p_ChatClient->GetSession().GetRoomSession(RoomID);
				
				if (ChatRoomSession != nullptr)
				{
					TArray<FMatrixMesssageEvents> RoomEventsDetails = ChatRoomSession->GetMessageEvents();
					for (auto RoomEvents : RoomEventsDetails)
					{
						SetNewMessage(RoomEvents);
					}
					
					//
					ChatRoomSession->d_EventOnNewMessagesReceived.AddUObject(this, &UProfileBarWidget::OnMessageResponseReceived);
					//
					//UnRead Event Notify
					if (ChatRoomSession->HasNewUnreadNotifications())
					{
						//TODO::Set Button as Selected Image
						SetupSelectedProfile();
					}
					//

					//New Friend Set As Selected  
					if (bIsCheckRequest)
					{
						SetupSelectedProfile();
					}


					//Read Evnet Notify
					ChatRoomSession->d_EventOnNewUnreadEvents.AddUObject(this, &UProfileBarWidget::OnReadEventsNotify);
					//

					//Show Online Status
					FString OnlineUserID = p_ChatClient->GetSession().GetFriendUserIDForRoom(RoomID);
					EMatrixPresence OnlineStatus = p_ChatClient->GetSession().GetPresenceForUser(OnlineUserID);
					if (OnlineStatus == EMatrixPresence::Online)
					{
						OnlineNotifyImage->SetVisibility(ESlateVisibility::Visible);
					}
					///
				}
			}
		}
}


void UProfileBarWidget::OnReadEventsNotify(bool bIsCheckNotify)
{
	if (!bIsCheckNotify)
	{
		//TODO:: Show unSelected Button
		SetupUnSelectedProfile();

	}
	else
	{
		//TODO:: Show Selected Button
		SetupSelectedProfile();
	}
}


void UProfileBarWidget::SetProfileInfo(FTesserconUserInfo InUserInfo)
{
	
	if (UserNameTextBlock != nullptr)
	{
		FString Name = InUserInfo.FirstName + " " + InUserInfo.LastName;
		UserNameTextBlock->SetText(FText::FromString(Name));
	}
	if (ProfileImage != nullptr)
	{
		UAsyncTaskDownloadImage* DownloadedImageTask = NewObject<UAsyncTaskDownloadImage>();
		DownloadedImageTask->OnSuccess.AddDynamic(this, &UProfileBarWidget::OnGetTexture);
		DownloadedImageTask->Start(InUserInfo.ProfileImageURL);
	}
}

void UProfileBarWidget::OnGetTexture(UTexture2DDynamic* Texture)
{
	if (ProfileImage == nullptr) return;
    ProfileImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	ProfileImage->SetBrushFromTextureDynamic(Texture, true);
}


void UProfileBarWidget::OnMessageResponseReceived(FMatrixMesssageEvents RoomMessageEvent)
{
	SetNewMessage(RoomMessageEvent);
}

void UProfileBarWidget::SetNewMessage(FMatrixMesssageEvents RoomEvents)
{
	auto Index = RoomEvents.EventOrder.Last();	
	FString MessageTime;
	
	/*if (Index.Key == EJoinedRoomEventType::CallInvite)
	{
		Invite = RoomEvents.CallInvites[Index.Value];
	}
	if (Index.Key == EJoinedRoomEventType::CallHangup)
	{
		Hangup = RoomEvents.CallHangUps[Index.Value];
	}*/


	//TODO::Need a Rcalculate Call Event 
	if (Index.Key  == EJoinedRoomEventType::Message)
	{
		FMatrixTextMessage Message = RoomEvents.Messages[Index.Value];
		if (bIsCheckGroup)
		{
			FString Username = GetUsernameFromMatrixUsername(Message.Sender);
			TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
			if (TessClient != nullptr)
			{
				TesserconAPI::FOnUserInfoByUserIDResponseDelegate  RequestListCallback;
				RequestListCallback.BindLambda([this, Message](ETesserconError Error, FTesserconUserInfo InUserInfo)
					{
						if (Error == ETesserconError::Success)
						{
							FString MesssageWithSenderName = InUserInfo.FirstName + " : " + Message.Message;
							MessageTextBlock->SetText(FText::FromString(MesssageWithSenderName));
						}
					});
				TessClient->Getsession().GetUserInfoByUsername(Username, RequestListCallback);
			}

		}
		else
		{
			MessageTextBlock->SetText(FText::FromString(Message.Message));
		}
		MessageTime = FText::AsTime(Message.TimeStamp, EDateTimeStyle::Short).ToString();
					
	}
	else if (Index.Key == EJoinedRoomEventType::CallHangup )
	{
		FMatrixCallEvent Message = RoomEvents.CallHangUps[Index.Value];
		CallLogImage->SetVisibility(ESlateVisibility::Visible);
	   MessageTextBlock->SetText(FText::FromString("Missed Call"));
	   MessageTime = FText::AsTime(Message.TimeStamp, EDateTimeStyle::Short).ToString();
	
	}
	else if (Index.Key == EJoinedRoomEventType::CallInvite)
	{
		FMatrixCallEvent Message = RoomEvents.CallInvites[Index.Value];
		MessageTextBlock->SetText(FText::FromString("Call Decliend"));
		CallLogImage->SetVisibility(ESlateVisibility::Visible);
		MessageTime = FText::AsTime(Message.TimeStamp, EDateTimeStyle::Short).ToString();
	}
	else if (Index.Key == EJoinedRoomEventType::CallAnswer)
	{
		FMatrixCallEvent Message = RoomEvents.CallAnswers[Index.Value];
		CallLogImage->SetVisibility(ESlateVisibility::Visible);
		MessageTextBlock->SetText(FText::FromString(" Call "));
	}
	TimeTextBlock->SetText(FText::FromString(MessageTime));
}


void UProfileBarWidget::TeamProfileBar(TSharedPtr<MatrixRoomSession> InRoomSessionInfo)
{
	
	bIsCheckGroup = true;
	TeamRoomInfo = InRoomSessionInfo;
	//its usr to hide UserType TextBlock 
	UserTypeSizeBox->SetVisibility(ESlateVisibility::Collapsed);
	SetTeamInfo();

}


void UProfileBarWidget::TeamRequestProfileBar(FMatrixInvitedRoom InRoomInfo)
{
	p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
	if (p_ChatClient != nullptr)
	{
		bIsCheckGroup = true;
		bIsCheckRequest = true;
		TeamRequestRoomInfo = InRoomInfo;
		SetupSelectedProfile();
		UserTypeSizeBox->SetVisibility(ESlateVisibility::Collapsed);
		FString TeamName = InRoomInfo.Name;
		int Length = TeamName.Len() - 1;
		if (UserNameTextBlock != nullptr)
		{
			UserNameTextBlock->SetText(FText::FromString(TeamName));
		}
		if (TeamnameTextBlock != nullptr)
		{
			TeamnameTextBlock->SetText(FText::FromString(TeamName.LeftChop(Length).ToUpper()));
		}
	}
	
}

void UProfileBarWidget::SetTeamInfo()
{
	if (TeamRoomInfo != nullptr)
	{
		TArray<FString>Admin = TeamRoomInfo->GetAdmins();
		FString TeamName = TeamRoomInfo->GetRoomName();
		int Length = TeamName.Len()-1;

		if(UserNameTextBlock != nullptr)
		{
			UserNameTextBlock->SetText(FText::FromString(TeamName));
		}
		if (TeamnameTextBlock != nullptr)
		{
			TeamnameTextBlock->SetText(FText::FromString(TeamName.LeftChop(Length).ToUpper()));
		}

		if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
		{
			p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
			if (p_ChatClient != nullptr)
			{
				ChatRoomSession = p_ChatClient->GetSession().GetRoomSession(TeamRoomInfo->GetRoomID());
				ChatRoomSession->d_EventOnRoomNameChanged.AddUObject(this, &UProfileBarWidget::TeamNameChangeResponse);
				ChatRoomSession->d_EventOnNewMessagesReceived.AddUObject(this, &UProfileBarWidget::OnMessageResponseReceived);
				if (ChatRoomSession != nullptr)
				{
					TArray<FMatrixMesssageEvents> RoomEventsDetails = ChatRoomSession->GetMessageEvents();
					for (auto RoomEvents : RoomEventsDetails)
					{
						SetNewMessage(RoomEvents);
					}

					//UnRead Event Notify
					if (ChatRoomSession->HasNewUnreadNotifications())
					{
						SetupSelectedProfile();
					}
					//

					//Read Evnet Notify
					ChatRoomSession->d_EventOnNewUnreadEvents.AddUObject(this, &UProfileBarWidget::OnReadEventsNotify);
					//
				}
			}
		}
	}
}

void UProfileBarWidget::TeamNameChangeResponse(FString TeamName)
{
	if (UserNameTextBlock != nullptr)
	{
		UserNameTextBlock->SetText(FText::FromString(TeamName));
	}
}

void UProfileBarWidget::OnProfileButtonClicked()
{
	if (UserChatWindowClass != nullptr)
	{
		UChatWindowWidget* ChatWidget = CreateWidget<UChatWindowWidget>(GetWorld(), UserChatWindowClass);

		if (ChatWidget != nullptr)
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
						if (bIsCheckGroup)
						{
							if (bIsCheckRequest)
							{
								if (p_ChatClient != nullptr)
								{
									MatrixNativeAPI::FOnMatrixAPIResponseDelegate AcceptRoomResponseCallback;
									AcceptRoomResponseCallback.BindLambda([this, ChatWidget](EMatrixCoreError Error) {
										if (Error == EMatrixCoreError::Success)
										{
											ChatRoomSession = p_ChatClient->GetSession().GetRoomSession(TeamRequestRoomInfo.RoomID);
											ChatWidget->TeamChatWidgetSetup(ChatRoomSession, this);
											HideNotifyIcon();
										}
										});
									p_ChatClient->GetSession().JoinRoom(TeamRequestRoomInfo.RoomID, TeamRequestRoomInfo.bIsDirect, TeamRequestRoomInfo.Sender, AcceptRoomResponseCallback);
								}
							}
							else 
							{
								ChatWidget->TeamChatWidgetSetup(TeamRoomInfo, this);
								HideNotifyIcon();
							}
						}
						else
						{
							ChatWidget->FriendChatWidgetSetup(UserInfo, RoomID,bIsCheckRequest,this);
							HideNotifyIcon();
						}
						MenuWidget->SetChatWidgetSizeBox(ChatWidget);
						
					}
				}
			}
		}
	}
}


void UProfileBarWidget::ShowChatWidget(FTesserconUserInfo InUserInfo, FString InRoomID)
{
	bIsCheckGroup = false;
	UserInfo = InUserInfo;
	if (UserChatWindowClass != nullptr)
	{
		UChatWindowWidget* ChatWidget = CreateWidget<UChatWindowWidget>(GetWorld(), UserChatWindowClass);

		if (ChatWidget != nullptr)
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
					   ChatWidget->FriendChatWidgetSetup(UserInfo, InRoomID,false, this);
					   MenuWidget->SetChatWidgetSizeBox(ChatWidget);

					}
				}
			}
		}
	}
}

void UProfileBarWidget::SetBackButton()
{
	if (MenuWidget != nullptr)
	{
		if (bIsCheckGroup)
		{
			MenuWidget->ShowGroup();
		}
		else
		{
			MenuWidget->ShowChatMenu();
		}
	}
}


void UProfileBarWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UProfileBarWidget::SetupSelectedProfile()
{
	FString ImageUrlStr = FString::Printf(TEXT("/Game/BaseCore/Textures/MainMenu/MainMenuSideChat/SideChatUserchatBarUnread"));
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(ImageUrlStr)));
	FSlateBrush Brush;
	if (Texture != nullptr)
	{
		Brush.SetResourceObject(Texture);
		ProfileButton->WidgetStyle.SetNormal(Brush);
		ProfileButton->WidgetStyle.SetHovered(Brush);
		
	}
}

void UProfileBarWidget::SetupUnSelectedProfile()
{

	FString ImageUrlStr = FString::Printf(TEXT("/Game/BaseCore/Textures/MainMenu/MainMenuSideChat/SideChatUserChatBarDef"));
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(ImageUrlStr)));
	FSlateBrush Brush;
	if (Texture != nullptr)
	{
		Brush.SetResourceObject(Texture);
		ProfileButton->WidgetStyle.SetNormal(Brush);
		ProfileButton->WidgetStyle.SetHovered(Brush);

	}

}

void UProfileBarWidget::HideNotifyIcon()
{
	
    ChatRoomSession->UpdateFullyReadMarker();
	
}
