// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu/InGameMenuWidget.h"
//class
#include "UI/InGameMenu/MapMenuWidget.h"
#include "UI/InGameMenu/InGameMiniMapWidget.h"
#include "UI/HUD/InGameHUD.h"
#include "UI/MainMenu/worldMessageBodyWidget.h"
#include "UI/InGameMenu/NearbyChatWidget.h"
#include "UI/MainMenu/CallWidget.h"
#include "UI/TesserconButton.h"
//Components
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
//Timer
#include "TimerManager.h"

UInGameMenuWidget::UInGameMenuWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MapMenuWidgetBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/InGameMenu/MapMenuWidget_WBP"));
	if (MapMenuWidgetBPObj.Class != nullptr)
	{
		MapMenuWidgetClass = MapMenuWidgetBPObj.Class;
	}
	//Nearby Chat WBP
	static ConstructorHelpers::FClassFinder<UUserWidget> NeaybyWidgetBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/InGameMenu/NearByChatList_WBP"));
	if (NeaybyWidgetBPObj.Class != nullptr)
	{
		NeaybyWidgetClass = NeaybyWidgetBPObj.Class;
	}
	/*static ConstructorHelpers::FClassFinder<UUserWidget> NeaybyCallWidgetBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/OnCallWidget_WBP"));
	if (NeaybyCallWidgetBPObj.Class != nullptr)
	{
		NeaybyCallWidgetClass = NeaybyCallWidgetBPObj.Class;
	}*/
}

void UInGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MiniMapButton != nullptr)
	{
		MiniMapButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnMiniMapButtonClicked);
	}

	if (SidePanelNearbyButton != nullptr)
	{
		SidePanelNearbyButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnNearbyChatButtonClicked);
	}

	if (MessageEditableTextBox != nullptr)
	{
		MessageEditableTextBox->OnTextCommitted.AddDynamic(this, &UInGameMenuWidget::OnMessageContentCommitted);
	}

	SetNearByCall();
}
void UInGameMenuWidget::SetNearByCall()
{
	if (OnCallWidgetclass != nullptr)
	{
		UCallWidget* NearByCallWidget = CreateWidget<UCallWidget>(GetWorld(), OnCallWidgetclass);
		if (NearByCallWidget != nullptr)
		{
			NearByCallWidget->NearBySetup();
			SetCallBarSizeBox(NearByCallWidget);
		}
	}

}
//LoadChannel Session call by join nearby channel event
void UInGameMenuWidget::LoadChannelSession(ETesserconError Error)
{
	if (Error == ETesserconError::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("ChannelSession.EventTextMessageReceived"));
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		if (TessClient != nullptr)
		{
			IChannelSession& ChannelSession = TessClient->Getsession().GetChannelSessionFromKey(EVivoxChannelKey::NearByChannel);
			if (&ChannelSession != nullptr)
			{
				ChannelSession.EventTextMessageReceived.AddUObject(this, &UInGameMenuWidget::OnVivoxChannelMessageRecived);
			}
		}
	}
}


// Message Recived
void UInGameMenuWidget::OnVivoxChannelMessageRecived(const IChannelTextMessage& Message)
{
	if (RunningChatMessageBodyClass != nullptr)
	{
		UworldMessageBodyWidget* MessageBodyWidget = CreateWidget<UworldMessageBodyWidget>(GetWorld(), RunningChatMessageBodyClass);
		if (MessageBodyWidget != nullptr)
		{
			FString UserID = GetMatrixFullUserIDForUsername(Message.Sender().Name());


			MessageBodyWidget->SendMessageInfo(Message.Message(), UserID);
			RunningChatScrollBox->AddChild(MessageBodyWidget);
			RunningChatScrollBox->ScrollToEnd();
			UE_LOG(LogTemp, Log, TEXT("Message Received from %s:"), *Message.Message());
			UE_LOG(LogTemp, Warning, TEXT("time stamp at message received %s"), *FDateTime::Now().ToString());
		}
	}
}

void UInGameMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
	//TODO
}

void UInGameMenuWidget::OnNearbyChatButtonClicked()
{
	if (SidePanelNearbyButton->p_SelectedButton != nullptr)
	{
		if (SidePanelNearbyButton->p_SelectedButton == SidePanelNearbyButton)
		{
			SidePanelNearbyButton->SetNormal();
			SidePanelNearbyButton->p_SelectedButton = nullptr;
			ListWidgetSizeBox->ClearChildren();
			ChatWidgetSizeBox->ClearChildren();
			if (MenuBackgroundImage != nullptr)
				MenuBackgroundImage->SetVisibility(ESlateVisibility::Hidden);
			return;
		}
		else
		{
			SidePanelNearbyButton->p_SelectedButton->SetNormal();
		}
	}
	SidePanelNearbyButton->SetSelected();
	SidePanelNearbyButton->p_SelectedButton = SidePanelNearbyButton;

	if (MenuBackgroundImage != nullptr)
		MenuBackgroundImage->SetVisibility(ESlateVisibility::Visible);

	if (NeaybyWidgetClass != nullptr)
	{
		UNearbyChatWidget* NearbyChatListWidget = CreateWidget<UNearbyChatWidget>(GetWorld(), NeaybyWidgetClass);
		if (NearbyChatListWidget != nullptr)
		{
				NearbyChatListWidget->SetupChannelInfo("TesserconNearby");
				ListWidgetSizeBox->ClearChildren();
				ListWidgetSizeBox->AddChild(NearbyChatListWidget);
				ChatWidgetSizeBox->ClearChildren();
		}
	}
}

void UInGameMenuWidget::OnMiniMapButtonClicked()
{
	if (MapMenuWidgetClass != nullptr)
	{
		UMapMenuWidget* BoothListWidget = CreateWidget<UMapMenuWidget>(GetWorld(), MapMenuWidgetClass);
		if (BoothListWidget != nullptr)
		{
			if (MiniMap != nullptr)
			{
				BoothListWidget->SetupMap(MiniMap->m_Zoom, MiniMap->m_Dimension, MiniMap->p_MiniMapImage);
			}
			BlankSizeBox->AddChild(BoothListWidget);
		}
	}
}

void UInGameMenuWidget::OnMessageContentCommitted(const FText& InText, ETextCommit::Type InCommitType)
{
	FString Message = MessageEditableTextBox->GetText().ToString();
	if (!Message.IsEmpty())
	{
		if (InCommitType == ETextCommit::Type::OnEnter)
		{
			TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
			if (TessClient != nullptr)
			{
				VivoxCoreError ErrorStatus = TessClient->Getsession().SendTextMessage("TesserconGlobel", Message, EVivoxChannelKey::NearByChannel);
				if (ErrorStatus == VxErrorSuccess)
				{
					MessageEditableTextBox->SetText(FText::FromString(""));
				}
				else
				{//TODO:: Remove else Part After Testing 
					MessageEditableTextBox->SetText(FText::FromString("Fail"));
				}
			}
		}
	}
}




