// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MenuWidget.h"

//Tessercon
#include "TesserconCore.h"
//Matrix
#include "MatrixCore.h"

#include "BaseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UBaseMenuWidget : public UMenuWidget
{
	GENERATED_BODY()
public:
	UBaseMenuWidget(const FObjectInitializer& ObjectInitializer);

	void ShowChatListSizeBox();
	void SetChatWidgetSizeBox(UUserWidget* ProfileWidget);
	void SetBlankSizeBox(UUserWidget* ProfileWidget);
	void ShowChatMenu();
	void ShowGroup();
	void GetSettingsClass(TSubclassOf<UUserWidget>& InClass);
	void GetUserProfileClass(TSubclassOf<UUserWidget>& InClass);
	void GetRunningChatMessageBodyClass(TSubclassOf<UUserWidget>& InClass);
	void GetListOfChatUserBodyClass(TSubclassOf<UUserWidget>& InClass);
	void GetProfileBarClass(TSubclassOf<UUserWidget>& InClass);
	void HideCallNotification();
	void SetCallBarSizeBox(UUserWidget* ProfileWidget);

protected:

	UPROPERTY(meta = (BindWidget))
		class UImage* MenuBackgroundImage;

	//Side Menu Bar
	UPROPERTY(meta = (BindWidget))
		class UTesserconButton* SidePanelChatButton;
	UPROPERTY(meta = (BindWidget))
		class UTesserconButton* SidePanelGroupsButton;
	UPROPERTY(meta = (BindWidget))
		class UTesserconButton* SidePanelCallButton;
	UPROPERTY(meta = (BindWidget))
		class UTesserconButton* SidePanelActivityButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* SidePanelHelpButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* SidePanelSettingsButton;

	UPROPERTY(meta = (BindWidget))
		class UImage* ChatNotifyImage;
	UPROPERTY(meta = (BindWidget))
		class UImage* GroupNotifyImage;
	UPROPERTY(meta = (BindWidget))
		class UImage* ActivityNotifyImage;

	//Running Chat
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* RunningChatScrollBox;
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* MessageEditableTextBox;

	//MenuSizeBox
	UPROPERTY(meta = (BindWidget))
		class USizeBox* BlankSizeBox;
	UPROPERTY(meta = (BindWidget))
		class USizeBox* RunningChatSizeBox;
	
	UPROPERTY(meta = (BindWidget))
		class USizeBox* CallBarSizeBox;
	UPROPERTY(meta = (BindWidget))
		class USizeBox* ListWidgetSizeBox;
	UPROPERTY(meta = (BindWidget))
		class USizeBox* ChatWidgetSizeBox;
	UPROPERTY(meta = (BindWidget))
		class USizeBox* CallLogSizeBox;

	// MenuBar
	UFUNCTION()
		void OnSidePanelChatButtonClicked();
	UFUNCTION()
		void OnSidePanelGroupsButtonClicked();
	UFUNCTION()
		void OnSidePanelCallButtonClicked();
	UFUNCTION()
		void OnSidePanelActivityButtonClicked();
	UFUNCTION()
		void OnSidePanelHelpButtonClicked();
	UFUNCTION()
		void SidePanelSettingsButtonClicked();

	

	//Matrix
	MatrixClient* p_ChatClient;
	TSharedPtr<MatrixRoomSession> m_ChatRoomSession;
	

	//Team Request Recived then withount any Confirmation join a room. 
	void OnTeamRequestRecived(FMatrixInvitedRoom InRoomInfo);
	//Friend Request
	void OnChatRequestRecived(FMatrixInvitedRoom InRoomInfo);

	void OnTeamChatNotification(bool bIsCheckNotify);
	void  OnFriendChatNotification(bool bIsCheckNotify);

	void ShowNotificationImage();

	//call
	void OnResponseCallEvent(EMatrixCallEventType EventType, FString RoomID, FMatrixCallEvent CalledInfo);
	void OnResponseCallstate(FTesserconCallState CallStateInfo);
	
	void OnSpeakerAvailablityChanged(bool bIsSpeakerAvilabel);
	void OnMicAvailablityChanged(bool bIsMicAvilabel);


	

	TSubclassOf<UUserWidget> SettingsClass;
	TSubclassOf<UUserWidget> UserProfileClass;
	TSubclassOf<UUserWidget> RunningChatMessageBodyClass;
	TSubclassOf<UUserWidget> ListOfChatUserBodyClass;
	TSubclassOf<UUserWidget> ProfileBarClass;
	TSubclassOf<UUserWidget> CallInfoListClass;
	TSubclassOf<UUserWidget> OnCallWidgetclass;

	/*class UListOfChatUserWidget* m_ChatList;*/
	class UListOfChatUserWidget* m_GroupChatList;
	
	//Call Info
	class UCallInfoListWidget* CallNotifyWidget;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
