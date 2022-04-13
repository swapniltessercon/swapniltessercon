// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageDateWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UMessageDateWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	//set the date of messages in MessageDateTextBlock.
	void SetMessagesDate(FDateTime TimeStamp);

	//store the TimeStamp.
	FDateTime PreviousDate;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MessagesDateTextBlock;
	
};
