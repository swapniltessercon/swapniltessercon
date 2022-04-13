// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/BaseMenuWidget.h"

#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UMainMenuWidget : public UBaseMenuWidget
{
	GENERATED_BODY()


public:
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

private:
	//Conference
	UPROPERTY(meta = (BindWidget))
		class UButton* ConferenceButton;

	UPROPERTY(meta = (BindWidget))
		class UImage* EventBannerImage;

	//Avatar
	UPROPERTY(meta = (BindWidget))
		class UButton* AvatarButton;
	UFUNCTION()
		void OnAvatarButtonClicked();
	

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuWidgetSwitcher;
	UPROPERTY(meta = (BindWidget))
		class UButton*  BackButton;
	UFUNCTION()
		void OnBackButtonClicked();
	//


	//EnterBooth
	UPROPERTY(meta = (BindWidget))
		class UButton* EnterBoothButton;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CompanyNameBoothTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UImage* CompanyLogoBoothImage;
	
	//EnterEvent
	UPROPERTY(meta = (BindWidget))
		class UButton* EnterEventButton;

	//User Profile
	UPROPERTY(meta = (BindWidget))
		class UButton* UserProfileButton;
	UPROPERTY(meta = (BindWidget))
		class UImage* UserProfileImage;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserNameTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* DesignationTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UButton* BoothListButton;
	UPROPERTY(meta = (BindWidget))
		class USizeBox* UserProfileSizeBox;
	
	//Conference
	UFUNCTION()
		void OnConferenceButtonClicked();
	//UserProfile
	UFUNCTION()
		void OnUserProfileButtonClicked();

	UFUNCTION()
		void OnBoothListButtonClicked();

	UFUNCTION()
		void GetEventImage(UTexture2DDynamic* Texture);
	//EnterBooth
	UFUNCTION()
		void OnEnterBoothButtonClicked();

	//EnterEvent
	UFUNCTION()
		void OnEnterEventButtonClicked();

	FTesserconUserInfo m_LoggedInUserInfo;
	
	void SetUserProfile();
	
	UFUNCTION()
		void OnSetProfileTexture(UTexture2DDynamic* Texture);

	TSubclassOf<UUserWidget> SelectEventClass;
	TSubclassOf<UUserWidget> BoothListClass;
	TSubclassOf<UUserWidget> AvatarSelectorClass;

	class UAvatarSelectorWidget* SelectAvatarWidget;


	void OnTeamAddedResponseReceived(FString RoomID);

	void OnMessageResponseReceived(FMatrixMesssageEvents RoomMessageEvent);
	

	UFUNCTION()
		void OnMessageContentCommitted(const FText& InText, ETextCommit::Type InCommitType);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
