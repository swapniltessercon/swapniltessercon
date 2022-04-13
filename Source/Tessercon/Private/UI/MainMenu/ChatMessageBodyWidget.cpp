// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/ChatMessageBodyWidget.h"

//Module
#include "TesserconCore.h"
//Components
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"

void UChatMessageBodyWidget::UpdateMessageDetails(FString ProfileName, FString MessageBody, FDateTime ReceivedTime)
{
	this->SetVisibility(ESlateVisibility::Visible);
	FString MessageTime = FText::AsTime(ReceivedTime, EDateTimeStyle::Short).ToString();

	if (MessageBodyTextBlock != nullptr)
	{
		MessageBodyTextBlock->SetText(FText::FromString(MessageBody));
	}
	if (ProfileNameTextBlock != nullptr)
	{
		ProfileNameTextBlock->SetText(FText::FromString(ProfileName));
	}
	if (MessageTimeTextBlock != nullptr)
	{
		MessageTimeTextBlock->SetText(FText::FromString(MessageTime));
	}
}



void UChatMessageBodyWidget::SetMessageInfo(FString Sender, FString Message, FDateTime TimeStamp, bool bIsDirect)
{


	FString SenderUsername;
	SenderUsername = GetUsernameFromMatrixUsername(Sender);

	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

	if (TessClient != nullptr)
	{
		FString Username = TessClient->Getsession().GetLoggedInUserInfo().Username;
			

		if (SenderUsername != Username)
		{

			if (SpacerSizeBox != nullptr)
			{
				UHorizontalBoxSlot* SpacersSlot = Cast<UHorizontalBoxSlot>(SpacerSizeBox->Slot);
				if (SpacersSlot != nullptr)
				{
					SpacersSlot->Size = FSlateChildSize(ESlateSizeRule::Automatic);
				}
			}

			ChatBodyBorder->SetBrushColor(FLinearColor(0.006512, 0.006512, 0.006512, 1.000000));

			if (MessageBodyTextBlock != nullptr)
			{
				UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(MessageBodyTextBlock->Slot);
				if (VerticalBoxSlot != nullptr)
				{
					VerticalBoxSlot->SetPadding(FMargin(10.0, 00.0, 10.0, 00.0));

				}
			}
			if (ProfileNameTextBlock != nullptr)
			{
				UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(ProfileNameTextBlock->Slot);
				if (VerticalBoxSlot != nullptr)
				{
					VerticalBoxSlot->SetPadding(FMargin(0.0, 00.0, 10.0, 00.0));

				}
			}

			ProfileNameTextBlock->SetJustification(ETextJustify::Left);
			MessageBodyTextBlock->SetJustification(ETextJustify::Left);
			MessageBodyTextBlock->SetAutoWrapText(true);

			if (!bIsDirect)
			{
				//this->SetVisibility(ESlateVisibility::Collapsed);
				TesserconAPI::FOnUserInfoByUserIDResponseDelegate RequestInfoCallback;
				RequestInfoCallback.BindLambda([this, Message, TimeStamp](ETesserconError Error, FTesserconUserInfo InUserInfo)
					{
						if (Error == ETesserconError::Success)
						{
							UpdateMessageDetails(InUserInfo.FirstName, Message, TimeStamp);
							MessageTimeStamp = TimeStamp;
						}
					});

				TessClient->Getsession().GetUserInfoByUsername(SenderUsername, RequestInfoCallback);
			}
			else
			{
				UpdateMessageDetails(" ", Message, TimeStamp);
			}
		}
		else
		{
			if (!bIsDirect)
			{
				UpdateMessageDetails("ME", Message, TimeStamp);
				MessageTimeStamp = TimeStamp;
			}
			else
			{
				UpdateMessageDetails(" ", Message, TimeStamp);
			}
		}
	}

}


