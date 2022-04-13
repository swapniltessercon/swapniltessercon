// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/MenuWidget.h"

#include "TesserconCore.h"

#include "LoginMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API ULoginMenuWidget : public UMenuWidget
{
	GENERATED_BODY()

public:
	virtual void Setup(EInputModeType InputType) override;

	virtual void Teardown() override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* LoginCanvas;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* ForgotPasswordCanvas;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* EmailSentCanvas;

	UPROPERTY(meta = (BindWidget))
		class UButton* CloseButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* MinimizeButton;

	//Login Menu Widgets
	UPROPERTY(meta = (BindWidget))
		class UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* ForgotPasswordButton;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ForgotPasswordButtonText;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* LoginEmailEditableTextBox;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* LoginPasswordEditableTextBox;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* LoginEmailErrorText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* LoginPasswordErrorText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* LoginButtonText;

	UPROPERTY(meta = (BindWidget))
		class UImage* LoginEmailErrorImage;

	UPROPERTY(meta = (BindWidget))
		class UImage* LoginPasswordErrorImage;

	UPROPERTY(meta = (BindWidget))
		class UImage* LoginThrobberImage;

	//Forgot Password Menu Widgets
	UPROPERTY(meta = (BindWidget))
		class UButton* SendEmailButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BackButtonText;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* ForgotPasswordEmailEditableTextBox;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ForgotPasswordEmailErrorText;
	
	//Email Sent Menu Widgets
	UPROPERTY(meta = (BindWidget))
		class UButton* BackToLoginButton;

	UFUNCTION()
		void OnCloseButtonClicked();

	UFUNCTION()
		void OnMinimizeButtonClicked();

	UFUNCTION()
		void OnLoginButtonClicked();

	UFUNCTION()
		void OnForgotPasswordButtonClicked();

	UFUNCTION()
		void OnForgotPasswordButtonHovered();

	UFUNCTION()
		void OnForgotPasswordButtonUnhovered();

	UFUNCTION()
		void OnSendEmailButtonClicked();

	UFUNCTION()
		void OnBackButtonClicked();

	UFUNCTION()
		void OnBackButtonHovered();

	UFUNCTION()
		void OnBackButtonUnhovered();

	UFUNCTION()
		void OnBackToLoginButtonClicked();

	UFUNCTION()
		void OnEmailContentChanged(const FText& Text);

	UFUNCTION()
		void OnPasswordContentChanged(const FText& Text);

	void EnableLoginWidget();
	
	void DisableLoginWidget();
	
	void OnLoginResponse(ETesserconError Status);
};
