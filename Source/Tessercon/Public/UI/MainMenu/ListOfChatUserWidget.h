// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TesserconCoreCommon.h"
#include "RoomSession.h"
#include "MatrixCore.h"
#include "ListOfChatUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UListOfChatUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UListOfChatUserWidget(const FObjectInitializer& ObjectInitializer);
	void Setup(EChatType ChatType);

private:
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* SearchEditableTextBox;
	UPROPERTY(meta = (BindWidget))
		class UButton* NewFriendAddButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* CreateNewTeamButton;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HeadingTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* ListScrollBox;

	UFUNCTION()
		void OnSearchEditableTextBoxClicked(const FText& text);
	
	UFUNCTION()
		void OnNewFriendAddButtonClicked();
	UFUNCTION()
		void OnCreateNewTeamButtonClicked();


	//if you get any Friend added Response.
	//its create Profile Widget and added in Profile ScrollBox
	//and also added in FriendDisplayRoomsMap
	void OnFriendAddedResponce(FString  FriendID);


	//if you get Friend Remove Response.
	//its call the RemoveFriendFromList(FString FriendID) 
	void OnFriendRemoveResponce(FString  FriendID);
	
	
	

	void UpdateFriendChanges();
	void AddFriendInList(FString InRoomID, bool bIsRequest);
	void RemoveFriendFromList(FString FriendID);
	

	//
	void AddTeamInList(TSharedPtr<MatrixRoomSession > InRoomSessionInfo);
	void OnTeamAdded(FString RoomID);
	void OnTeamRemove(FString RoomID);
	void UpdateTeamChanges();
	FString EventPublicRoom;


	//Update Friend Request List
	void UpdateFriendRequestList();
	//
	void OnFriendRequestRecived(FMatrixInvitedRoom InRoomInfo);

	//Update Team  Request List
	void UpdateTeamRequestList();
	//
	void OnTeamRequestRecived(FMatrixInvitedRoom InRoomInfo);

	void AddTeamRequestInList(FMatrixInvitedRoom InRoomInfo);


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


	TSubclassOf<UUserWidget> SearchUserClass;
	TSubclassOf<UUserWidget> ProfileBarClass;
	TSubclassOf<UUserWidget> CreateTeamWidgetClass;
	class UBaseMenuWidget* MenuWidget;
	class MatrixClient* p_ChatClient;

	//stored the random Sequensce list with ProfileWidget.
	TMap<FString, class UProfileBarWidget*> FriendDisplayRoomsMap;
	TMap<FString, class UProfileBarWidget*> TeamDisplayRoomsMap;


	
};
