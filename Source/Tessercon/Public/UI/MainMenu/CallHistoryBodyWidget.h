// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatrixCore.h"
#include "CallHistoryBodyWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UCallHistoryBodyWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	void ReceiveCallEvent(EMatrixCallEventType EventType, FMatrixCallEvent Event);

private:

	FString CallInviteSenderID;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CallMessageTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TimeTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UImage* CallInfoImage;

	TMap<EMatrixCallEventType, FMatrixCallEvent> ReceivedCallEvents;

	void ReCalculateMessage();
	void SetMissedCallTexture();
	void SetDeclinedCallTexture();
	void SetCallDurationTexture();
	
};
