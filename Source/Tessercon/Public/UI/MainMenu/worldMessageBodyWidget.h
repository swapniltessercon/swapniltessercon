// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/BaseMenuWidget.h"
#include "UI/HUD/MainMenuHUD.h"
#include "worldMessageBodyWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UworldMessageBodyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SendMessageInfo(const FString& Message, const FString& Sender);
private:
	UPROPERTY(meta = (BindWidget))
		class UButton* SenderProfileInfoButton;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* SenderNameTextBlock;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MessageTextBlock;
	UFUNCTION()
		void OnSenderProfileInfoButtonClicked();

	FString MessageSender;
	class UBaseMenuWidget* MenuWidget;
	bool bIsFlag=false;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
};
