// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TesserconCoreCommon.h"
#include "RoomSession.h"
#include "TeamInfoProfileBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UTeamInfoProfileBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetProfile(class UTeamCreateWidget* InParent, FTesserconUserInfo InUserInfo);
	void SetAddMembersProfile(FTesserconUserInfo InUserInfo, TSharedPtr<MatrixRoomSession> InRoomSessionInfo);
	void SetTeamMemberProfile(FTesserconUserInfo InUserInfo);
	void SetSearchMemberProfile(FTesserconUserInfo InUserInfo);
	void SetNearByProfile(FTesserconUserInfo InUserInfo);

	void SetSpeakerButtonVisible(FString InParticipantUserID);
	void SetSpeakerButtonCollapsed();

	
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserNameTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserTypeTextBolock;
private:
	UPROPERTY(meta = (BindWidget))
		class UButton* ToggleButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* SpeakerButton;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ToggleTextBlock;
	
	UPROPERTY(meta = (BindWidget))
		class UImage* ProfileImage;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* DesignationTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UComboBoxString* ProfileInfoComboBoxString;


	UFUNCTION()
		void OnToggleButtonClicked();
	UFUNCTION()
		void OnSpeakerButtonClicked();
	UFUNCTION()
		void OnSelectOption(FString SelectedOption, ESelectInfo::Type SelectType);
	UFUNCTION()
		void OnGetTexture(UTexture2DDynamic* Texture);


	void PopulateComboBox();

	void PopulateTeamMemberInfoComboBox();
	
	void SetToggleStatus();


	void SetProfileInfo();
	void ShowProfile();
	void ShowChatWindowWidget();

	

	class UTeamCreateWidget* Parent;
	
	FTesserconUserInfo UserInfo;

	int AddMembersCount;
	

	TSharedPtr<MatrixRoomSession> TeamRoomInfo;


	bool bIsMicEnable;
	
	
	//here need a two channels 1.voicechannel 2.Nearbychannel 
	EVivoxChannelKey ChannelKey;

	//Speaker

	
	void SetUserSpeakerButtonImage();
	void SetSpeakerMuteButtonImage();


protected:
	virtual void NativeConstruct() override;
	class MatrixClient* p_ChatClient;

	

	
};
