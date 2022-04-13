// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TesserconCore.h"
#include "MatrixCore.h"
#include "SearchNewUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API USearchNewUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USearchNewUserWidget(const FObjectInitializer& ObjectInitializer);
	
private:
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* SearchEditableTextBox;
	UPROPERTY(meta = (BindWidget))
		class UButton* SearchButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* CloseButton;
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* SearchUserScrollBox;
	UPROPERTY(meta = (BindWidget))
		class USizeBox* SearchListSizeBox;
	UPROPERTY(meta = (BindWidget))
		class UImage* BGImage;

	
	UFUNCTION()
		void OnSearchButtonClicked();

	UFUNCTION()
		void OnSearchEditableTextBoxClicked(const FText& text);

	void AddEventParticipant(FString UserID);


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void GetAllEventMembers();
	class MatrixClient* p_ChatClient;
	TSubclassOf<UUserWidget> TeamUserProfileClass;
	class UTeamInfoProfileBarWidget* ProfileRow;
	
	TMap<FString, class UTeamInfoProfileBarWidget*> ListOfWorldMemberMap;


	UFUNCTION()
		FEventReply OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent);


};
