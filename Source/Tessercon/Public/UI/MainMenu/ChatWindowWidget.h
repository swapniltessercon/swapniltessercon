// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//class

#include "UI/MainMenu/BaseChatWindowWidget.h"
#include "UI/MainMenu/ProfileBarWidget.h"
#include "ChatWindowWidget.generated.h"


UENUM()
enum class ESelectedGroupInfo : uint8
{
	GroupInfo,
    Members,
	AddMembers
};
/**
 * 
 */
UCLASS()
class TESSERCON_API UChatWindowWidget : public UBaseChatWindowWidget
{
	GENERATED_BODY()

public:
	void FriendChatWidgetSetup(FTesserconUserInfo InUserInfo, FString InRoomID, bool bIsRequest,UProfileBarWidget* InParent);
	void TeamChatWidgetSetup(TSharedPtr<MatrixRoomSession> InRoomSessionInfo, UProfileBarWidget* InParent);
private:
		UPROPERTY(meta = (BindWidget))
			class UButton* BackButton;
		UPROPERTY(meta = (BindWidget))
			class UTextBlock* DesignationTextBlock;
		UPROPERTY(meta = (BindWidget))
			class UTextBlock* UsernameTextBlock;

		UPROPERTY(meta = (BindWidget))
			class UTextBlock* TeamTextBlock;
	
		UPROPERTY(meta = (BindWidget))
			class UImage* ProfileInfoBGImage;
		UPROPERTY(meta = (BindWidget))
			class UImage* ProfileImage;
		UPROPERTY(meta = (BindWidget))
			class UImage* OnlineNotifyImage;
		UPROPERTY(meta = (BindWidget))
			class UComboBoxString* InfoListComboBoxString;
		UPROPERTY(meta = (BindWidget))
			class UWidgetSwitcher* ChatWidgetSwitcher;


		//Call 
		UPROPERTY(meta = (BindWidget))
			class UImage* MicNotifyImage;


		//RequestProfile
		UPROPERTY(meta = (BindWidget))
			class UImage* RequestProfileImage;
		UPROPERTY(meta = (BindWidget))
			class UTextBlock* RequestNameTextBlock;
		UPROPERTY(meta = (BindWidget))
			class UTextBlock* RequestDesignationTextBlock;
		UPROPERTY(meta = (BindWidget))
			class UButton* RejectButton;
		UPROPERTY(meta = (BindWidget))
			class UButton* AcceptButton;


		UFUNCTION()
			void OnBackButtonClicked();
		UFUNCTION()
			void OnGetTexture(UTexture2DDynamic* Texture);
		UFUNCTION()
			void OnGetRequetProfileTexture(UTexture2DDynamic* Texture);

		UFUNCTION()
			void OnSelectOption(FString SelectedOption, ESelectInfo::Type SelectType);

		void SetFriendRequestProfile(FTesserconUserInfo InUserInfo);

		void SetFriendProfileInfo(FTesserconUserInfo InUserInfo);
		void SetTeamProfileInfo();
		void PopulateComboBox();
		void PopulateTeamComboBox();
		void ShowProfile();
		void CreateTeamInfoWidget();
		void TeamNameChangeResponse(FString TeamName);

		//Team Info
		void ShowGroupInfo(ESelectedGroupInfo Type);



		//here Getting Event bIsMicAvailable is false Its means your Mic device Disconnected otherwise true. 
		void OnMicAvailablityChanged(bool bIsMicAvailable);

		//here Getting Event bIsSpeakerAvailable is false Its means your Speaker device Disconnected otherwise true. 
		void OnSpeakerAvailablityChanged(bool bIsSpeakerAvailable);


		UFUNCTION()
			void OnRejectButtonClicked();

		UFUNCTION()
			void OnAcceptButtonClicked();
		
		class UProfileBarWidget* Parent;
		FString RoomID;
		FTesserconUserInfo UserInfo;

		TSharedPtr<MatrixRoomSession> TeamRoomInfo;

		class UTeamInfoWidget* TeamInfoWidget;
		class UBaseMenuWidget* MenuWidget;

		
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	

 


};
