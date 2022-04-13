// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MainMenu/ChatWindowWidget.h"
#include "TeamInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UTeamInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetGroupInfo(ESelectedGroupInfo Type, TSharedPtr<MatrixRoomSession> InRoomSessionInfo);
	UTeamInfoWidget(const FObjectInitializer& ObjectInitializer);
private:
	//
	UPROPERTY(meta = (BindWidget))
		class UButton* GroupInfoButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* MembersButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* AddMembersButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* LeaveGroupButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* CloseButton;
	//
	UPROPERTY(meta = (BindWidget))
		class UImage* BGImage;
	UPROPERTY(meta = (BindWidget))
		class UImage* TeamProfileImage;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TeamNameTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* TeamNameChangeEditableTextBox;
	
	//Horizant box and DeletGroupButton its  visible only for admin.
	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* AdminButtonHorizontalBox;
	//save and cancel button part of horizontalBox
	UPROPERTY(meta = (BindWidget))
		class UButton* SaveChangesButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* CancelButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* DeleteGroupButton;
	//
	//Editable Only for Admin 
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* DescriptionEditableTextBox;
	//
	//Members
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* MembersListScrollBox;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MembersCountTextBlock;
	///
	//AddMemebr
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* SearchEditableTextBox;
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* AddMembersScrollBox;
	//

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* InfoWidgetSwitcher;


	UFUNCTION()
		void OnGroupInfoButtonClicked();
	UFUNCTION()
		void OnMembersButtonClicked();
	UFUNCTION()
		void OnAddMembersButtonClicked();

	//Leave Group 
	UFUNCTION()
		void OnLeaveGroupButtonClicked();
	UFUNCTION()
		void OnSaveChangesButtonClicked();
	UFUNCTION()
		void OnCancelButtonClicked();
	UFUNCTION()
		void OnDeleteGroupButtonClicked();

	UFUNCTION()
		void OnTeamNameEditableTextBoxChanged(const FText& text);
	FText ClampText(FText Text, int32 MaxCount);


	// its display the Group Info;
	void ShowGroupInfo();
	// Show Team Members
	void ShowMembers();
	//Show Friend list those are not added in team. 
	void ShowAddMembers();
	TSharedPtr<MatrixRoomSession> TeamRoomInfo;

	//Sort Friendlist   
	void SortFriendListForAddMembers();
	TMap<FString, TSharedPtr<MatrixRoomSession>> SortedFriendList;

	//Its Display the Join Memebers
	void ShowTeamMembersList(TArray<FString> InRoomMembers);

	TSubclassOf<UUserWidget> TeamUserProfileClass;


	UFUNCTION()
		FEventReply OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent);


	void OnResponseCallstate(FTesserconCallState CallStateInfo);
	//CallState 
	TMap<FString, class UTeamInfoProfileBarWidget*> DisplayProfileMap;

	void OnVivoxChannelParticipantAdded(const IParticipant& Participant);
	void OnVivoxChannelParticipantRemove(const IParticipant& Participant);

	void UpdateTeamParticipant();

protected:
	virtual void NativeConstruct() override;
	class MatrixClient* p_ChatClient;

	
	
};
