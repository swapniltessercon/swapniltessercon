// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"


#include "TesserconCoreCommon.h"
#include "TesserconCore.h"
#include "MatrixCore.h"
#include "RoomSession.h"

#include "CallWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UCallWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	//here check the Callstatus and doing respective operations.
	//such as if callstatus is dialling then Time Block,speaker and mic button collapsed.  
	void Setup(FTesserconCallState CallStateInfo);

	//send the CallID.
	FString GetCallID();

	//its Function used For the Convert the int to text Format in Two Digits Format
//MinimumIntegralDigits = 2 //Its means Minimum 2 digit int Count
//MaximumIntegralDigits = 2 //Its means Miximum 2 digit int Count
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToText (integer)", AdvancedDisplay = "1", BlueprintAutocast), Category = "Utilities|Text")
		FText Conv_IntToFText(int32 Value, bool bAlwaysSign = false, bool bUseGrouping = true, int32 MinimumIntegralDigits = 2, int32 MaximumIntegralDigits = 2);

	void NearBySetup();


protected:
	void NativeConstruct();

	void NativeDestruct();
private:

	UPROPERTY(meta = (BindWidget))
		class UButton* EndCallButton;

	UPROPERTY(meta = (BindWidget))
		class UTesserconButton* MuteSpeakerButton;

	UPROPERTY(meta = (BindWidget))
		class UTesserconButton* MuteMicButton;
	
	UPROPERTY(meta = (BindWidget))
		class UButton* ChatButton;

	UPROPERTY(meta = (BindWidget))
		class UImage* UserProfileImage;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserNameText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MinuteTextBlock;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* SecondTextBlock;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* DialingTextBlock;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* CallTimeHorizontalBox;



	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TeamGroupIconNameText;

	UFUNCTION()
		void OnEndCallClicked();

	UFUNCTION()
		void OnChatClicked();

	UFUNCTION()
		void OnMuteSpeakerClicked();

	UFUNCTION()
		void OnMuteMicClicked();

	UFUNCTION()
		void OnGetTexture(UTexture2DDynamic* Texture);


	////If Hangup Event is recieved widget gets NativeDestruct is called
	//void OnResponseCallEvent(EMatrixCallEventType EventType, FString RoomID, FMatrixCallEvent CallInfo);

	////Here we get event of mic device disconnected.
	//void OnMicAvailablityChanged(bool bIsMicAvailable);

	////Here we get event of speaker device disconnected.
	//void OnSpeakerAvailablityChanged(bool bIsSpeakerAvailable);


	void OnCallAction();

	void SetCallState(FTesserconCallState InCallStateInfo);

	void UpdateCallDurationTimer();

	void OnResponseCallState(FTesserconCallState CallStateInfo);

	void SetProfileImage(FString ImageUrl);

	void SetCallInfo();


	class MatrixClient* m_ChatClient;
	FTimerHandle UpdateCallTimer;
	FString RoomName;
	int Minute = 0;
	int Seconds = 0;
	bool bIsMicEnable = true;
	bool bIsSpeakarEnable = true;
	FTesserconCallState CallStateInfoStored;


	bool bIsCallInfoTextSet = false;

};
