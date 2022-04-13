// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "TesserconCoreCommon.h"
//#include "TesserconCoreErrors.h"
//#include "RoomSession.h"
//#include "TesserconCore.h"
//#include "MatrixCore.h"
#include "UI/BaseMenuWidget.h"
#include "UI/HUD/MainMenuHUD.h"
#include "ProfileBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UProfileBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UProfileBarWidget(const FObjectInitializer& ObjectInitializer);
	void FriendProfileBar(FString InRoomID,bool bIsRequest);
	void TeamProfileBar(TSharedPtr<MatrixRoomSession> InRoomSessionInfo);
	void SetBackButton();
	void ShowChatWidget(FTesserconUserInfo InUserInfo, FString RoomID);
	//Team Request Profile 
	void TeamRequestProfileBar(FMatrixInvitedRoom InRoomInfo);


	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserNameTextBlock;
	
private:
	UPROPERTY(meta = (BindWidget))
		class UButton* ProfileButton;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TimeTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UImage* ProfileImage;
	UPROPERTY(meta = (BindWidget))
		class UImage* OnlineNotifyImage;

	UPROPERTY(meta = (BindWidget))
		class UImage* CallLogImage;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MessageTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserTypeTextBolock;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TeamnameTextBlock;


	UPROPERTY(meta = (BindWidget))
		class USizeBox* UserTypeSizeBox;

	UFUNCTION()
		void OnProfileButtonClicked();

	void SetProfileInfo(FTesserconUserInfo InUserInfo);
	UFUNCTION()
		void OnGetTexture(UTexture2DDynamic* Texture);

	void SetNewMessage(FMatrixMesssageEvents RoomEvents);
	void TeamNameChangeResponse(FString TeamName);

	//Read Noftify
	void OnReadEventsNotify(bool bIsCheckNotify);


	//
	void SetupSelectedProfile();
	//
	void SetupUnSelectedProfile();

	void HideNotifyIcon();

	void OnMessageResponseReceived(FMatrixMesssageEvents RoomMessageEvent);




	FString RoomID;
	class MatrixClient* p_ChatClient;
	FTesserconUserInfo UserInfo;
	class UBaseMenuWidget* MenuWidget;
	TSharedPtr<MatrixRoomSession> ChatRoomSession;
	TSharedPtr<MatrixRoomSession> TeamRoomInfo;
	FMatrixInvitedRoom TeamRequestRoomInfo;

	TSubclassOf<UUserWidget> UserChatWindowClass;

	bool bIsCheckGroup;
	bool bIsCheckRequest;

	void SetTeamInfo();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	


	

};
