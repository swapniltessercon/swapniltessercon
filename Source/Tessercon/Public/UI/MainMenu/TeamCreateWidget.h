// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TesserconCoreCommon.h"
#include "TeamCreateWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UTeamCreateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTeamCreateWidget(const FObjectInitializer& ObjectInitializer);
	void AddInTeamList(FTesserconUserInfo InUserInfo);
private:

	UPROPERTY(meta = (BindWidget))
		class UImage* ImageGroupIconImage;

	UPROPERTY(meta = (BindWidget))
		class UButton* ChangeGroupIconButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* CancelButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* NextButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* TeamCreateButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* TeamCreateCancelButton;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* TeamNameEditableTextBox;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* SearchEditableTextBox;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* AvatarHorizontalBox;
	UPROPERTY(meta = (BindWidget))
		class USizeBox* ListSizeBox;
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* SearchListScrollBox;

	
	UFUNCTION()
		void OnChangeGroupIconButtonClicked();
	UFUNCTION()
		void OnNextButtonClicked();
	UFUNCTION()
		void OnTeamCreateButtonClicked();
	/*UFUNCTION()
		void OnTeamCreateCancelButtonClicked();
	UFUNCTION()
		void OnCancelButtonClicked();*/

	UFUNCTION()
		void OnSearchEditableTextBoxChanged(const FText& text);
	UFUNCTION()
		void OnTeamNameEditableTextBoxChanged(const FText& text);

	FText ClampText(FText Text, int32 MaxCount);

	class MatrixClient* p_ChatClient;

	TMap<FString, FTesserconUserInfo> MemberInfo;

	TSubclassOf<UUserWidget> TeamUserProfileClass;
	

protected:
	virtual void NativeConstruct() override;
	
	
};
