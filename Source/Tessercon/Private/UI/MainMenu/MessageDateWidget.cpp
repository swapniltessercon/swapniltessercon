// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/MessageDateWidget.h"

#include "Components/TextBlock.h"

void UMessageDateWidget::SetMessagesDate(FDateTime TimeStamp)
{
	PreviousDate = TimeStamp;
	FDateTime Today = FDateTime::UtcNow();
	FString TodayDate = FText::AsDate(Today, EDateTimeStyle::Default).ToString();
	FString Date = FText::AsDate(TimeStamp, EDateTimeStyle::Default).ToString();

	if (TodayDate == Date)
	{
		Date = "Today";
	}
	MessagesDateTextBlock->SetText(FText::FromString(Date));
}