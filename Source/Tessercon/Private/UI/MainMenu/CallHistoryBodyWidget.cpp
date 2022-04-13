// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/CallHistoryBodyWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UCallHistoryBodyWidget::ReceiveCallEvent(EMatrixCallEventType EventType, FMatrixCallEvent Event)
{
	ReceivedCallEvents.Add(EventType, Event);
	if (ReceivedCallEvents.Contains(EMatrixCallEventType::Invite) || ReceivedCallEvents.Contains(EMatrixCallEventType::Answer))
	{
		ReCalculateMessage();
	}
}

void UCallHistoryBodyWidget::ReCalculateMessage()
{
	if (CallMessageTextBlock != nullptr && ReceivedCallEvents.Contains(EMatrixCallEventType::Invite))
	{
		FMatrixCallEvent Invite = *ReceivedCallEvents.Find(EMatrixCallEventType::Invite);
		FString CallerName = Invite.Sender;
		FString Time = FText::AsTime(Invite.TimeStamp, EDateTimeStyle::Short).ToString();
		//FString Time = .ToString();

		if (ReceivedCallEvents.Contains(EMatrixCallEventType::Hangup) && ReceivedCallEvents.Contains(EMatrixCallEventType::Answer))
		{
			// display caller name

			FMatrixCallEvent Answer = *ReceivedCallEvents.Find(EMatrixCallEventType::Answer);
			FMatrixCallEvent Hangup = *ReceivedCallEvents.Find(EMatrixCallEventType::Hangup);
			FTimespan CallDuration = (Hangup.TimeStamp - Answer.TimeStamp);
			int SecondsTime = CallDuration.GetSeconds();
			int MinuteTime = CallDuration.GetMinutes();
			int HoursTime = CallDuration.GetHours();
			FString Duration = ((FString::FromInt(HoursTime)) + ":" + (FString::FromInt(MinuteTime)) + ":" + (FString::FromInt(SecondsTime)));
			CallMessageTextBlock->SetText(FText::FromString("Call Duration at "));
			TimeTextBlock->SetText(FText::FromString(Duration));
			SetCallDurationTexture();

		}

		if (ReceivedCallEvents.Num() == 1)
		{
			// display call name and ringing
			// TODO: if the caller name is same as the logged in user 
			//CallMessageTextBlock->SetText(FText::FromString(Time + "   " + CallerName + "   " + "Connecting"));
			// TODO: else ringing

			CallMessageTextBlock->SetText(FText::FromString("Connecting..."));
			TimeTextBlock->SetText(FText::FromString(Time));

		}

		if (ReceivedCallEvents.Contains(EMatrixCallEventType::Answer) && ReceivedCallEvents.Num() == 2)
		{
			// display caller name and ongoing
			FMatrixCallEvent Answer = *ReceivedCallEvents.Find(EMatrixCallEventType::Answer);

			//CallMessageTextBlock->SetText(FText::FromString(Time + "   " + CallerName + "   " + "Ongoing"));
			SetCallDurationTexture();
			CallMessageTextBlock->SetText(FText::FromString("Ongoing at "));
			TimeTextBlock->SetText(FText::FromString(Time));

		}

		if (ReceivedCallEvents.Contains(EMatrixCallEventType::Hangup) && ReceivedCallEvents.Num() == 2)
		{
			// display caller name (missed call or call declined)
			FMatrixCallEvent Hangup = *ReceivedCallEvents.Find(EMatrixCallEventType::Hangup);

			// will have to check the logged in matrix user name and also compare that too
			if (Invite.Sender == Hangup.Sender)
			{
				//CallMessageTextBlock->SetText(FText::FromString(Time + "   " + CallerName + "   " + "Missed Call"));

				CallMessageTextBlock->SetText(FText::FromString("Missed Call at "));
				TimeTextBlock->SetText(FText::FromString(Time));
				SetMissedCallTexture();
			}
			else
			{
				//CallMessageTextBlock->SetText(FText::FromString(Time + "   " + CallerName + "   " + "Call Declined"));
				SetDeclinedCallTexture();
				CallMessageTextBlock->SetText(FText::FromString("Call Declined at "));
				TimeTextBlock->SetText(FText::FromString(Time));

			}

		}
	}

}

void UCallHistoryBodyWidget::SetMissedCallTexture()
{
	FString Path = "/Game/BaseCore/Textures/MainMenu/CallBar/MainMenuCallLogMissed";
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));
	CallInfoImage->SetBrushFromTexture(Texture);
}

void UCallHistoryBodyWidget::SetDeclinedCallTexture()
{ 
	//TODO 
	//FString Path = "/Game/Textures/Main_Menu/chat/icons/call_declined";
	FString Path = "/Game/BaseCore/Textures/MainMenu/CallBar/MainMenuCallLogMissed";
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));
	CallInfoImage->SetBrushFromTexture(Texture);
}

void UCallHistoryBodyWidget::SetCallDurationTexture()
{
	FString Path = "/Game/BaseCore/Textures/MainMenu/CallBar/CallLogIncoming";
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));
	CallInfoImage->SetBrushFromTexture(Texture);
}
