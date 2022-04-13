// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/BaseMenuWidget.h"
#include "UI/HUD/MainMenuHUD.h"
#include "VivoxCoreCommon.h"
#include "CallerNotifyProfileWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UCallerNotifyProfileWidget : public UUserWidget
{
	GENERATED_BODY()

public:
public:
	void InitalSetup(class UCallInfoListWidget* InParent, FMatrixCallEvent InviteInfo, FString InRoomID);
	//Set Caller profile in widget get userinfo using username.
	//InviteInfo  : its structure of Call Details such as CallID,EventID,CallerID[sender] and TimeStamp. 

	

private:

	UPROPERTY(meta = (BindWidget))
		class UImage* UserProfileImage;
	UPROPERTY(meta = (BindWidget))
		class UButton* AcceptCallButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* RejectCallButton;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserNameTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* DesignationTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserTypeTextBolock;


	UFUNCTION()
		void OnRejectCallButtonClicked();
	//using tessercon Core Module call the HangupCallInvite Function with Roomid ,MatrixInviteInfo.id and FOnTesserconClientResponseDelegate.

	UFUNCTION()
		void OnAcceptCallButtonClicked();
	//using tessercon Core Module call the Answercall() Function with Roomid and MatrixInviteInfo.


	UFUNCTION()
		void OnGetTexture(UTexture2DDynamic* Texture);
	//its call by if start the download Image by UAsyncTaskDownloadImage class.
	//

	void OnResponseCallEvent(EMatrixCallEventType EventType, FString InRoomID, FMatrixCallEvent CallInfo);
	//here if you getting Hangup event then Remove the widget from Parent[Remove the Widget].
	//EvnetType : Invite,Hangup, Answer.
	//CallInfo  : its structure of Call Details. 

	void SetProfileInfo(FTesserconUserInfo InUserInfo);
	//Set Profile using UserInfo.

	class UCallInfoListWidget* Parent;
	FTimerHandle CallExpireTimer;
	FMatrixCallEvent Invite;
	FString RoomID;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
