// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/worldMessageBodyWidget.h"
//
#include "UI/Profile/UserProfileWidget.h"


//Components
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"



void UworldMessageBodyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SenderProfileInfoButton != nullptr)
	{
		SenderProfileInfoButton->OnClicked.AddDynamic(this, &UworldMessageBodyWidget::OnSenderProfileInfoButtonClicked);
	}
}

void UworldMessageBodyWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UworldMessageBodyWidget::SendMessageInfo(const FString& Message, const FString& Sender)
{
	FString UserName ;
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		UserName = TessClient->Getsession().GetLoggedInUserInfo().Username;
	}
	//TODO:: Need a Mange Mock Data 
	MessageSender = Sender;

	FString SendUserID = GetUsernameFromMatrixUsername(Sender);
	
    if (Sender == "@tess285684752781164923075739541638533203179:comms.tessercon.com")
	{
		SenderNameTextBlock->SetText(FText::FromString("Exhibitor"));
		//SenderProfileInfoButton->SetIsEnabled(false);
	}
	else if (Sender == "@tess296884408640382890146014818978608119200:comms.tessercon.com")
	{
		SenderNameTextBlock->SetText(FText::FromString("Nagesh"));
	}
	else if(Sender == "@tess117336637117798955933579942743100164077:comms.tessercon.com")
	{
		SenderNameTextBlock->SetText(FText::FromString("Attendee"));
	}
	else if (UserName == SendUserID)
	{
		SenderNameTextBlock->SetText(FText::FromString("You"));
		SenderProfileInfoButton->SetIsEnabled(false);
	}

	MessageTextBlock->SetText(FText::FromString(Message));
}

void UworldMessageBodyWidget::OnSenderProfileInfoButtonClicked()
{
	
		if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
		{
			TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

			if (PlayerController != nullptr)
			{
				ABaseHUD* HUD = Cast<ABaseHUD>(PlayerController->GetHUD());

				if (HUD != nullptr)
				{
					FString Username = GetUsernameFromMatrixUsername(MessageSender);
					MenuWidget = Cast<UBaseMenuWidget>(HUD->GetCurrentMenuWidget());
					if (MenuWidget != nullptr)
					{
						TSubclassOf<UUserWidget> UserProfileClass;
						MenuWidget->GetUserProfileClass(UserProfileClass);
						UUserProfileWidget* UserProfileWidget = CreateWidget<UUserProfileWidget>(GetWorld(), UserProfileClass);
						if (UserProfileWidget != nullptr)
						{
							TesserconAPI::FOnUserInfoByUserIDResponseDelegate UserInfoCallBack;
							UserInfoCallBack.BindLambda([this, UserProfileWidget](ETesserconError ErrorStatus, FTesserconUserInfo InUserInfo)
								{
									UserProfileWidget->SetProfile(InUserInfo, MessageSender);
									MenuWidget->SetBlankSizeBox(UserProfileWidget);
								}
							);
							/*FString Username = GetUsernameFromMatrixUsername(MessageSender);*/
							TessClient->Getsession().GetUserInfoByUsername(Username, UserInfoCallBack);
						}
					}
				}
			}
		}
	
}