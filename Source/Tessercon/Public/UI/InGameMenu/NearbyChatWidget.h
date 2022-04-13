// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/InGameMenu/InGameMenuWidget.h"
#include "NearbyChatWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UNearbyChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupChannelInfo(FString InChannelName);
	UNearbyChatWidget(const FObjectInitializer& ObjectInitializer);


private:
	FString ChannelName;
	FString SelfUserID;
	TSubclassOf<UUserWidget> ProfileWidgetClass;

	void UpdateParticipants();
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* ListScrollBox;
	/*TSharedPtr<IChannelSession> ChannelSession;*/
	FString LoggedInUserName;

	void AddParticipant(FString UserID);
	void RemoveParticipant(FString UserID);
	void OnVivoxChannelParticipantAdded(const IParticipant& Participant);
	void OnVivoxChannelParticipantRemove(const IParticipant& Participant);

	TMap<FString, class UTeamInfoProfileBarWidget*> WorldListMap;

protected:
	virtual void NativeDestruct() override;

	
};
