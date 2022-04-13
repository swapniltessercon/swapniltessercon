// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatMessageBodyWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UChatMessageBodyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//here create dynamically Left side Chatbox [Sender Side]
	void SetMessageInfo(FString Sender, FString Message, FDateTime TimeStamp, bool bIsDirect);

	//send the Message TimeStamp
	FDateTime GetMessageTimeStamp()
	{
		return MessageTimeStamp;
	}
	
private:

	UPROPERTY(meta = (BindWidget))
		class UVerticalBox* BorderVerticalBox;

	UPROPERTY(meta = (BindWidget))
		class UBorder* ChatBodyBorder;

	UPROPERTY(meta = (BindWidget))
		class USizeBox* SpacerSizeBox;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MessageBodyTextBlock;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MessageTimeTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ProfileNameTextBlock;
	FDateTime MessageTimeStamp;

	void UpdateMessageDetails(FString ProfileName, FString MessageBody, FDateTime ReceivedTime);
};
