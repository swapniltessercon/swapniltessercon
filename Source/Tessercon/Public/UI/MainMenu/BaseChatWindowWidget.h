// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TesserconCoreCommon.h"
#include "RoomSession.h"
#include "MatrixCore.h"
#include "TesserconCore.h"
#include "BaseChatWindowWidget.generated.h"

//Visual Studio 
#ifdef SendMessage
#undef SendMessage
#endif

/**
 * 
 */
UCLASS()
class TESSERCON_API UBaseChatWindowWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UBaseChatWindowWidget(const FObjectInitializer& ObjectInitializer);
	//If you want to get the type class of a blueprint BP, you can get it through ConstructorHelpers::FClassFinder<>,

	void BaseSetup(FString RoomID, bool InbISDirect);
	// Friend,Team, and World chat operation done here with respective RoomId

protected:

	virtual void NativeConstruct() override;
	//Native Construct Bind Event
	virtual void NativeDestruct() override;
	//Native Destruct Unbind Event

	TSharedPtr<MatrixRoomSession> m_ChatRoomSession;
	class MatrixClient* p_ChatClient;

	TSubclassOf<UUserWidget> UserProfileClass;
	TSubclassOf<UUserWidget> TeamInfoClass;
	UPROPERTY(meta = (BindWidget))
		class UButton* CallButton;

private:

	class UVerticalBox* m_dateBox = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* MessagesScrollBox;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* MessageEditableTextBox;

	UPROPERTY(meta = (BindWidget))
		class UButton* SendMessageButton;



	UFUNCTION()
		void OnUserScrolledCallBack(float CurrentOffset);
	//get Scroll Offset

	UFUNCTION()
		void OnMessageTextChanged(const FText& text);
	//.

	UFUNCTION()
		void OnSendButtonClicked();
	///used For If you click on send Button then Send the Message.

	UFUNCTION()
		void OnCallButtonClicked();
	//Place Call Using Call Button Event 

	UFUNCTION()
		void OnMessageContentCommitted(const FText& InText, ETextCommit::Type InCommitType);
	//used For If you Press Enter Button then send Message 

	void SendMessage(FString TextMessage);
	//Using RoomSestion Call the SendMessage function with message and MatrixApiReponseDelegate.

	void OnMessageResponseReceived(FMatrixMesssageEvents Message);
	// Call the UPdateUIForEvnetBatch() with argument bIsFetchingPastMessages is false

	void OnPastMessageResponseReceived(FMatrixMesssageEvents RoomMessageEvent);
	// Call the UPdateUIForEventBatch() with argument bIsFetchingPastMessages is true

	void ScrollOnSelfMessageReceivedCallback(FString InEventID);
	//FullyReadMark

	void UpdateUIForEventBatch(FMatrixMesssageEvents RoomEvents, bool bIsPastMessageEvent);
	//  Create MessagebodyWidget, DateTimeWidget and CallHistoryWidget Respective TimeStamp
	//  add in chat Widget Scroll box

	

	FString m_ChatRoomID;

	bool bIsNoMoreFetchMessages = true;
	//if no more messgaes then set as false	

	bool bIsFetchingPastMessages = false;
	//get New message response set as false.
	// Old Message fetch then set as true

	bool bIsDirect;
	// true set for Friend Room 
	// false set for Team Room 

	//Message Body class 
	TSubclassOf<UUserWidget> m_MessageBodyClass;
	//Call History Class such as MissedCall .
	TSubclassOf<UUserWidget> m_CallHistoryBodyClass;
	//Date Widget class 
	TSubclassOf<UUserWidget> m_MessagesDateBarClass;
	

	

	// EventID, Widget*
	TMap<FString, UWidget*> EventWidgetsMap;

	// callid, widget*
	TMap<FString, UWidget*> CallLogMapBuffer;

	FDateTime m_PreviousDate;
	// its stored the PreviousDate Respective Messgae, call Timestamp

	//Call state
	void OnResponseCallstate(FTesserconCallState CallStateInfo);
	
};
