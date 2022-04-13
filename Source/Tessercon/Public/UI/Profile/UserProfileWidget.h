// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TesserconCore.h"
#include "UserProfileWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UUserProfileWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void SetProfile(FTesserconUserInfo InUserInfo);
	void SetProfile(FTesserconUserInfo InUserInfo, FString Sender);
	
private:

	UPROPERTY(meta = (BindWidget))
		class UImage* BGBlurImage;


	//Common ProfileInfo
	UPROPERTY(meta = (BindWidget))
		class UImage* ProfileImage;
	
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* DesignationTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserNameTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserTypeTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AboutTextBlock;
	/*UPROPERTY(meta = (BindWidget))
		class UImage* CompanyLogoImage;*/
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CompantyNameTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CompanyWebsiteTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UButton* ProfileCloseButton;
	//
	// Visible only for Loggedin User
	UPROPERTY(meta = (BindWidget))
		class USizeBox* AvatarEditButtonSizeBox;
	UPROPERTY(meta = (BindWidget))
		class UButton* ProfileEditAvatarButton;
	//
	//Chat And Call Button Visible for another user 
	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* ChatAndCallHorizontalBox;
	UPROPERTY(meta = (BindWidget))
		class UButton* ProfileChatButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* ProfileCallButton;

	UPROPERTY(meta = (BindWidget))
		class UImage* MicNotifyIconImage;
	//
	//Business Card Details
	UPROPERTY(meta = (BindWidget))
		class USizeBox* BusinessCardDetailsSizeBox;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PhoneNumberTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* EmailTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AddressTextBlock;
	//
	//Request Button
	UPROPERTY(meta = (BindWidget))
		class USizeBox* RequestBussinessCardSizeBox;
	UPROPERTY(meta = (BindWidget))
		class UButton* RequestBusinesCardButton;
	//
	//Block Button
	UPROPERTY(meta = (BindWidget))
		class USizeBox* BlockBarSizeBox;
	UPROPERTY(meta = (BindWidget))
		class UButton* ProfileBlockButton;
	//



	UFUNCTION()
		void OnProfileEditAvatarButtonClicked();
	UFUNCTION()
		void OnProfileChatButtonClicked();
	UFUNCTION()
		void OnProfileCallButtonClicked();
	UFUNCTION()
		void OnRequestBusinesCardButtonClicked();
	UFUNCTION()
		void OnProfileBlockButtonClicked();
	UFUNCTION()
		void OnProfileCloseButtonClicked();

	//LoggedInInfo
	FTesserconUserInfo LoggedInUserInfo;
	//
	FTesserconUserInfo UserInfo;

	void SetCommonProfile(FTesserconUserInfo InUserInfo);
	//

	UFUNCTION()
		void OnSetProfileTexture(UTexture2DDynamic* Texture);
	
	/*UFUNCTION()
		void OnSetCompanyLogoTexture(UTexture2DDynamic* Texture);*/

	FString UserID;
	TArray<FString> UserIds;

	 // ProfileBarClass;

	UFUNCTION()
		FEventReply OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent);



	FString m_RoomID;

	//here Getting Event bIsMicAvailable is false Its means your Mic device Disconnected otherwise true. 
	void OnMicAvailablityChanged(bool bIsMicAvailable);

	//here Getting Event bIsSpeakerAvailable is false Its means your Speaker device Disconnected otherwise true. 
	void OnSpeakerAvailablityChanged(bool bIsSpeakerAvailable);




protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	class MatrixClient* p_ChatClient;


	
};
