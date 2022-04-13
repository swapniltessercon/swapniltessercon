// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/BaseChatWindowWidget.h"



//
#include "UI/HUD/InGameHUD.h"
//class
#include "UI/MainMenu/ChatMessageBodyWidget.h"
#include "UI/MainMenu/MessageDateWidget.h"
#include "UI/MainMenu/CallHistoryBodyWidget.h"


//Components
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"


UBaseChatWindowWidget::UBaseChatWindowWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	
	static ConstructorHelpers::FClassFinder<UUserWidget> MessageBodyBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/ChatMessageBody_WBP"));
	if (MessageBodyBPObj.Class != nullptr)
	{
		m_MessageBodyClass = MessageBodyBPObj.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> MessagesDateBarBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/MessageDate_WBP"));
	if (MessagesDateBarBPObj.Class != nullptr)
	{
		m_MessagesDateBarClass = MessagesDateBarBPObj.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> UserProfileBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/UserProfile_WBP"));
	if (UserProfileBPObj.Class != nullptr)
	{
		UserProfileClass = UserProfileBPObj.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> TeamInfoBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/TeamInfo_WBP"));
	if (TeamInfoBPObj.Class != nullptr)
	{
		TeamInfoClass = TeamInfoBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> CallHistoryBodyBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/CallHistoryChat_WBP"));
	if (CallHistoryBodyBPObj.Class != nullptr)
	{
		m_CallHistoryBodyClass = CallHistoryBodyBPObj.Class;
	}
}

void UBaseChatWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SendMessageButton != nullptr)
	{
		SendMessageButton->OnClicked.AddDynamic(this, &UBaseChatWindowWidget::OnSendButtonClicked);
	}

	if (MessageEditableTextBox != nullptr)
	{
		MessageEditableTextBox->OnTextCommitted.AddDynamic(this, &UBaseChatWindowWidget::OnMessageContentCommitted);
		MessageEditableTextBox->OnTextChanged.AddDynamic(this, &UBaseChatWindowWidget::OnMessageTextChanged);
	}

	if (MessagesScrollBox != nullptr)
	{
		MessagesScrollBox->OnUserScrolled.AddDynamic(this, &UBaseChatWindowWidget::OnUserScrolledCallBack);
	}

	if (CallButton != nullptr)
	{
		CallButton->OnClicked.AddDynamic(this, &UBaseChatWindowWidget::OnCallButtonClicked);
	}


	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().d_EventOnCallStateChanged.AddUObject(this, &UBaseChatWindowWidget::OnResponseCallstate);
		}
	}
}

void UBaseChatWindowWidget::BaseSetup(FString RoomID,bool InbISDirect)
{
	bIsDirect = InbISDirect;
	m_ChatRoomID = RoomID;
	if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
	{
		p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();

		if (p_ChatClient != nullptr)
		{
			m_ChatRoomSession = p_ChatClient->GetSession().GetRoomSession(RoomID);
		}
		if (m_ChatRoomSession != nullptr)
		{
			m_ChatRoomSession->d_EventOnNewMessagesReceived.AddUObject(this, &UBaseChatWindowWidget::OnMessageResponseReceived);
			m_ChatRoomSession->d_EventOnPastMessagesReceived.AddUObject(this, &UBaseChatWindowWidget::OnPastMessageResponseReceived);
			m_ChatRoomSession->d_EventOnSelfMessageReceived.AddUObject(this, &UBaseChatWindowWidget::ScrollOnSelfMessageReceivedCallback);

			TArray<FMatrixMesssageEvents> RoomEventsDetails = m_ChatRoomSession->GetMessageEvents();
			for (auto RoomEvents : RoomEventsDetails)
			{
				UpdateUIForEventBatch(RoomEvents, false);
				MessagesScrollBox->ScrollToEnd();
			}
		}
	}
}

void UBaseChatWindowWidget::NativeDestruct()
{
	if (m_ChatRoomSession != nullptr)
	{
		m_ChatRoomSession->d_EventOnNewMessagesReceived.RemoveAll(this);
		m_ChatRoomSession->d_EventOnPastMessagesReceived.RemoveAll(this);
		m_ChatRoomSession->d_EventOnSelfMessageReceived.RemoveAll(this);
	}

	Super::NativeDestruct();
}


void UBaseChatWindowWidget::OnMessageTextChanged(const FText& text)
{
	/*if (ChatRoomSession != nullptr)
	{
		ChatRoomSession->SendTypingNotification(true);
	}*/
}

void UBaseChatWindowWidget::OnMessageContentCommitted(const FText& InText, ETextCommit::Type InCommitType)
{
	if (InCommitType == ETextCommit::Type::OnEnter)
	{
		FString Message = InText.ToString();
		if (Message.Len() < 1) return;
		this->SendMessage(Message);
	}
}



// Send Button Bind
void UBaseChatWindowWidget::OnSendButtonClicked()
{
	if (m_ChatRoomSession != nullptr && MessageEditableTextBox != nullptr)
	{
		FString Message = MessageEditableTextBox->GetText().ToString();
		if (Message != "")
		{
			this->SendMessage(Message);
		}
	}
}


// Call  Button Bind
void UBaseChatWindowWidget::OnCallButtonClicked()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconSession* TessSession = &FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient()->Getsession();
		if (TessSession != nullptr)
		{
			if (TessSession->IsSpeakerAvailable() && TessSession->IsMicAvailable())
			{
				TessSession->PlaceCall(m_ChatRoomID);
				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
				if (PlayerController != nullptr)
				{
					AInGameHUD* InGameHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
					if (InGameHUD != nullptr)
					{
						//Set TarnsmissionMode as none
						TessSession->UnSetNearByChannelTransmissionMode();
					}
				}
			}
		}
	}
}

void UBaseChatWindowWidget::SendMessage(FString TextMessage)
{
	if (m_ChatRoomSession != nullptr)
	{
		MatrixNativeAPI::FOnMatrixAPIResponseDelegate MessageResponceCallBack;
		MessageResponceCallBack.BindLambda([this](EMatrixCoreError Error) {
				if (Error == EMatrixCoreError::Success)
				{
					MessageEditableTextBox->SetText(FText::FromString(""));
				}
			});
	     m_ChatRoomSession->SendMessage(TextMessage, MessageResponceCallBack);
	}
}

void UBaseChatWindowWidget::OnMessageResponseReceived(FMatrixMesssageEvents RoomMessageEvent)
{
	UpdateUIForEventBatch(RoomMessageEvent, false);


}

void UBaseChatWindowWidget::OnPastMessageResponseReceived(FMatrixMesssageEvents RoomMessageEvent)
{
	bIsFetchingPastMessages = false;
	UpdateUIForEventBatch(RoomMessageEvent, true);
}

void UBaseChatWindowWidget::UpdateUIForEventBatch(FMatrixMesssageEvents RoomEvents, bool bIsPastMessageEvent)
{
	TArray<TPair<EJoinedRoomEventType, int32>> RoomEventOrderList;
	TArray<UWidget*> NewWidgets;

	for (auto Pair : RoomEvents.EventOrder)
	{
		if (Pair.Key == EJoinedRoomEventType::Message)
		{
			FMatrixTextMessage Message = RoomEvents.Messages[Pair.Value];
			if (m_MessageBodyClass != nullptr)
			{
				UChatMessageBodyWidget* MessageBodyWidget = CreateWidget<UChatMessageBodyWidget>(GetWorld(), m_MessageBodyClass);
				if (MessageBodyWidget != nullptr)
				{
					// TODO: update the send message info to -> set message info
					MessageBodyWidget->SetMessageInfo(Message.Sender, Message.Message, Message.TimeStamp, bIsDirect);
					if (Message.TimeStamp.GetDate() != m_PreviousDate)
					{
						if (m_MessagesDateBarClass != nullptr)
						{
							UMessageDateWidget* MessagesDateWidget = CreateWidget<UMessageDateWidget>(GetWorld(), m_MessagesDateBarClass);
							if (MessagesDateWidget != nullptr)
							{
								m_PreviousDate = Message.TimeStamp.GetDate();
								FString Date = FText::AsDate(Message.TimeStamp, EDateTimeStyle::Default).ToString();
								MessagesDateWidget->SetMessagesDate(m_PreviousDate);
								NewWidgets.Add(MessagesDateWidget);
							}
						}
					}
					NewWidgets.Add(MessageBodyWidget);
					EventWidgetsMap.Add(Message.EventID, MessageBodyWidget);
				}
			}
		}
		else if (Pair.Key == EJoinedRoomEventType::CallAnswer || Pair.Key == EJoinedRoomEventType::CallHangup || Pair.Key == EJoinedRoomEventType::CallInvite)
		{
			FMatrixCallEvent CallEvent;
			EMatrixCallEventType CallEventType;
			if (Pair.Key == EJoinedRoomEventType::CallAnswer)
			{
				CallEvent = RoomEvents.CallAnswers[Pair.Value];
				CallEventType = EMatrixCallEventType::Answer;
			}
			else if (Pair.Key == EJoinedRoomEventType::CallInvite)
			{
				CallEvent = RoomEvents.CallInvites[Pair.Value];
				CallEventType = EMatrixCallEventType::Invite;
			}
			else
			{
				CallEvent = RoomEvents.CallHangUps[Pair.Value];
				CallEventType = EMatrixCallEventType::Hangup;
			}

			if (CallLogMapBuffer.Contains(CallEvent.CallID))
			{
				UCallHistoryBodyWidget* CallHistoryBodyWidget = Cast<UCallHistoryBodyWidget>(*CallLogMapBuffer.Find(CallEvent.CallID));
				if (CallHistoryBodyWidget != nullptr)
				{
					CallHistoryBodyWidget->ReceiveCallEvent(CallEventType, CallEvent);
					EventWidgetsMap.Add(CallEvent.EventID, CallHistoryBodyWidget);
					CallLogMapBuffer.Add(CallEvent.CallID, CallHistoryBodyWidget);
					if (CallEventType == EMatrixCallEventType::Invite)
					{
						NewWidgets.Add(CallHistoryBodyWidget);
					}
				}
			}
			else
			{
				// create the widget
				if (m_CallHistoryBodyClass != nullptr)
				{
					UCallHistoryBodyWidget* CallHistoryBodyWidget = CreateWidget<UCallHistoryBodyWidget>(GetWorld(), m_CallHistoryBodyClass);
					if (CallHistoryBodyWidget != nullptr)
					{
						CallHistoryBodyWidget->ReceiveCallEvent(CallEventType, CallEvent);
						EventWidgetsMap.Add(CallEvent.EventID, CallHistoryBodyWidget);
						CallLogMapBuffer.Add(CallEvent.CallID, CallHistoryBodyWidget);
						if (CallEventType == EMatrixCallEventType::Invite)
						{
							if (CallEvent.TimeStamp.GetDate() != m_PreviousDate)
							{
								if (m_MessagesDateBarClass != nullptr)
								{
									UMessageDateWidget* MessagesDateWidget = CreateWidget<UMessageDateWidget>(GetWorld(), m_MessagesDateBarClass);
				
									if (MessagesDateWidget != nullptr)
									{
										m_PreviousDate = CallEvent.TimeStamp.GetDate();
										FString Date = FText::AsDate(CallEvent.TimeStamp, EDateTimeStyle::Default).ToString();
										MessagesDateWidget->SetMessagesDate(m_PreviousDate);
										NewWidgets.Add(MessagesDateWidget);
									}
								}
							}
							NewWidgets.Add(CallHistoryBodyWidget);
						}
					}
				}
			}
		}
	}


	if (MessagesScrollBox != nullptr)
	{
		TArray<UWidget*> ExistingWidgets = MessagesScrollBox->GetAllChildren();
		UWidget* FirstWidgetBeforeCombining = nullptr;
		UWidget* EndWidgetBeforeCombining = nullptr;
		if (ExistingWidgets.Num() > 0)
		{
			FirstWidgetBeforeCombining = ExistingWidgets[0];
			EndWidgetBeforeCombining = ExistingWidgets.Last();
		}
		TArray<UWidget*> CombinedWidgets;
		if (bIsPastMessageEvent)
		{
			for (auto GetWidget : NewWidgets)
			{
				UMessageDateWidget* NewBodyWidget = Cast<UMessageDateWidget>(GetWidget);
				UChatMessageBodyWidget* NewMessageBody = Cast<UChatMessageBodyWidget>(NewWidgets[0]);
				UMessageDateWidget* ExistingBodyWidget = Cast<UMessageDateWidget>(FirstWidgetBeforeCombining);
				if (NewBodyWidget != nullptr && ExistingBodyWidget != nullptr)
				{
					if (NewBodyWidget->PreviousDate == ExistingBodyWidget->PreviousDate)
					{
						ExistingWidgets.Remove(FirstWidgetBeforeCombining);
					}
				}
				if (NewMessageBody != nullptr && ExistingBodyWidget != nullptr)
				{
					if (m_PreviousDate == ExistingBodyWidget->PreviousDate)
					{
						ExistingWidgets.Remove(FirstWidgetBeforeCombining);
						CombinedWidgets.Add(FirstWidgetBeforeCombining);
					}
				}
			}
			for (auto NewWidget : NewWidgets)
			{
				CombinedWidgets.Add(NewWidget);
			}
			for (auto ExistingWidget : ExistingWidgets)
			{
				CombinedWidgets.Add(ExistingWidget);
			}
		}
		else
		{
			CombinedWidgets = ExistingWidgets;
			if (NewWidgets.Num())
			{
				for (auto ExistingWidget : CombinedWidgets)
				{
					UMessageDateWidget* NewBodyWidget = Cast<UMessageDateWidget>(NewWidgets[0]);
					UMessageDateWidget* ExistingBodyWidget = Cast<UMessageDateWidget>(ExistingWidget);
					if (NewBodyWidget != nullptr && ExistingBodyWidget != nullptr)
					{
						if (NewBodyWidget->MessagesDateTextBlock->GetText().ToString() == ExistingBodyWidget->MessagesDateTextBlock->GetText().ToString())
						{
							FDateTime ExistingDate = ExistingBodyWidget->PreviousDate;
							ExistingBodyWidget->SetMessagesDate(ExistingDate);
						}
					}
				}
			}
			for (auto NewWidget : NewWidgets)
			{
				CombinedWidgets.Add(NewWidget);
			}
		}
		MessagesScrollBox->ClearChildren();
		for (auto CombinedWidget : CombinedWidgets)
		{
			MessagesScrollBox->AddChild(CombinedWidget);
		}
		if (bIsPastMessageEvent)
		{
			if (FirstWidgetBeforeCombining != nullptr)
			{
				MessagesScrollBox->ScrollWidgetIntoView(FirstWidgetBeforeCombining);
			}
			else
			{
				MessagesScrollBox->ScrollToStart();
			}
		}
		else
		{
			if (EndWidgetBeforeCombining != nullptr)
			{
				MessagesScrollBox->ScrollWidgetIntoView(EndWidgetBeforeCombining);
			}
			else
			{
				MessagesScrollBox->ScrollToEnd();
			}
		}
	}
}

void UBaseChatWindowWidget::OnUserScrolledCallBack(float CurrentOffset)
{
	float EndOffset = MessagesScrollBox->GetScrollOffsetOfEnd();
	UE_LOG(LogTemp, Warning, TEXT("User update responseScroll offset= %f"), CurrentOffset)
		if (CurrentOffset == 0 && bIsNoMoreFetchMessages && !bIsFetchingPastMessages)
		{
			if (m_ChatRoomSession != nullptr)
			{
				bIsFetchingPastMessages = true;
				EMatrixGetMessagesStatus MessageListStatus = m_ChatRoomSession->GetPastMessages();
				if (MessageListStatus == EMatrixGetMessagesStatus::NoMoreMessagesToFetch)
				{
					bIsNoMoreFetchMessages = false;
					bIsFetchingPastMessages = false;
				}
			}
		}
		else if (CurrentOffset == EndOffset)
		{
			if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
			{
				if (m_ChatRoomSession != nullptr)
				{
					m_ChatRoomSession->UpdateFullyReadMarker();;
				}
			}
		}
}

void UBaseChatWindowWidget::ScrollOnSelfMessageReceivedCallback(FString InEventID)
{
	if (EventWidgetsMap.Contains(InEventID))
	{
		MessagesScrollBox->ScrollWidgetIntoView(*EventWidgetsMap.Find(InEventID));
	}
}


//Call State
void UBaseChatWindowWidget::OnResponseCallstate(FTesserconCallState CallStateInfo)
{
	if (CallStateInfo.CallStatus == ETesserconCallStatus::OnCall)
	{
		CallButton->SetIsEnabled(false);
	}
	else if (CallStateInfo.CallStatus == ETesserconCallStatus::Available)
	{
		CallButton->SetIsEnabled(true);
	}
}