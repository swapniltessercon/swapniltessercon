// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoginMenu/LoginMenuWidget.h"

#include "GameFramework/GameUserSettings.h"

#include<regex>
#include<windows.h>

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"

#include "Engine/GameEngine.h"
#include "EngineGlobals.h"
#include "SlateCore.h"
#include "TesserconBlueprintFunctionLib.h"

void ULoginMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//Close and Minimize Bindings
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &ULoginMenuWidget::OnCloseButtonClicked);
	}

	if (MinimizeButton)
	{
		MinimizeButton->OnClicked.AddDynamic(this, &ULoginMenuWidget::OnMinimizeButtonClicked);
	}

	//Buttons Bindings
	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &ULoginMenuWidget::OnLoginButtonClicked);
	}

	if (ForgotPasswordButton)
	{
		ForgotPasswordButton->OnClicked.AddDynamic(this, &ULoginMenuWidget::OnForgotPasswordButtonClicked);
		ForgotPasswordButton->OnHovered.AddDynamic(this, &ULoginMenuWidget::OnForgotPasswordButtonHovered);
		ForgotPasswordButton->OnUnhovered.AddDynamic(this, &ULoginMenuWidget::OnForgotPasswordButtonUnhovered);
	}

	if (SendEmailButton)
	{
		SendEmailButton->OnClicked.AddDynamic(this, &ULoginMenuWidget::OnSendEmailButtonClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &ULoginMenuWidget::OnBackButtonClicked);
		BackButton->OnHovered.AddDynamic(this, &ULoginMenuWidget::OnBackButtonHovered);
		BackButton->OnUnhovered.AddDynamic(this, &ULoginMenuWidget::OnBackButtonUnhovered);
	}

	if (BackToLoginButton)
	{
		BackToLoginButton->OnClicked.AddDynamic(this, &ULoginMenuWidget::OnBackToLoginButtonClicked);
	}

	if (LoginEmailEditableTextBox != nullptr && LoginPasswordEditableTextBox != nullptr)
	{
		LoginEmailEditableTextBox->OnTextChanged.AddDynamic(this, &ULoginMenuWidget::OnEmailContentChanged);
		LoginPasswordEditableTextBox->OnTextChanged.AddDynamic(this, &ULoginMenuWidget::OnPasswordContentChanged);
	}

	if (ForgotPasswordEmailEditableTextBox != nullptr)
	{
		ForgotPasswordEmailEditableTextBox->OnTextChanged.AddDynamic(this, &ULoginMenuWidget::OnEmailContentChanged);
	}

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().d_EventLoginStatusResponse.AddUObject(this, &ULoginMenuWidget::OnLoginResponse);
		}
	}
}

void ULoginMenuWidget::OnEmailContentChanged(const FText& Text)
{
	FString Path = "/Game/BaseCore/Textures/Login/LoginInputDefault";
	UObject* TextureObject = LoadObject<UTexture2D>(NULL, *Path, NULL, LOAD_None, NULL);
	FSlateBrush Brush;
	Brush.SetResourceObject(TextureObject);

	LoginEmailEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);

	ForgotPasswordEmailEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);

	LoginEmailErrorImage->SetVisibility(ESlateVisibility::Hidden);
}

void ULoginMenuWidget::OnPasswordContentChanged(const FText& Text)
{
	FString Path = "/Game/BaseCore/Textures/Login/LoginInputDefault";
	UObject* TextureObject = LoadObject<UTexture2D>(NULL, *Path, NULL, LOAD_None, NULL);
	FSlateBrush Brush;
	Brush.SetResourceObject(TextureObject);

	LoginPasswordEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);
}

void ULoginMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().d_EventLoginStatusResponse.RemoveAll(this);
		}
	}
}

void ULoginMenuWidget::Setup(EInputModeType InputType)
{
	Super::Setup(InputType);

	//Window code from https://nerivec.github.io/old-ue4-wiki/pages/game-user-settings.html
	int32 Width = 384, Height = 407;
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (Settings != nullptr)
	{
		Settings->SetScreenResolution(FIntPoint(Width, Height));
	}

	//Settings->ConfirmVideoMode();
	Settings->SetFullscreenMode(EWindowMode::Type::Windowed);
	Settings->ApplySettings(false);
}

void ULoginMenuWidget::Teardown()
{
	int32 Width=0, Height=0;
	Width = GetSystemMetrics(SM_CXSCREEN);
	Height = GetSystemMetrics(SM_CYSCREEN);

	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (Settings != nullptr)
	{
		Settings->RequestResolutionChange(Width, Height, EWindowMode::Type::Windowed, false);
	}

	Settings->ConfirmVideoMode();
	Settings->SetFullscreenMode(EWindowMode::Type::Fullscreen);
	Settings->SaveSettings();
	Super::Teardown();
}

void ULoginMenuWidget::OnCloseButtonClicked()
{
	FGenericPlatformMisc::RequestExit(false);
}

void ULoginMenuWidget::OnMinimizeButtonClicked()
{
	UGameEngine* gameEngine = Cast<UGameEngine>(GEngine);

	if (gameEngine)
	{
		TSharedPtr<SWindow> windowPtr = gameEngine->GameViewportWindow.Pin();
		SWindow* window = windowPtr.Get();
		if (window)
		{
			window->Minimize();
		}
	}
}

bool Is_Valid_Email(const FString& email)
{
	const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
	return std::regex_match(std::string(TCHAR_TO_UTF8(*email)), pattern);
}

void ULoginMenuWidget::OnLoginButtonClicked()
{
	this->DisableLoginWidget();

	if (LoginEmailEditableTextBox == nullptr) return;
	if (LoginPasswordEditableTextBox == nullptr) return;

	FString Email = LoginEmailEditableTextBox->GetText().ToString();
	FString Password = LoginPasswordEditableTextBox->GetText().ToString();

	FString Path = "/Game/BaseCore/Textures/Login/LoginInputError";;
	UObject* TextureObject = LoadObject<UTexture2D>(NULL, *Path, NULL, LOAD_None, NULL);
	FSlateBrush Brush;
	Brush.SetResourceObject(TextureObject);

	if (Is_Valid_Email(Email) && Password != "")
	{
		if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
		{
			TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

			if (TessClient != nullptr)
			{
				TessClient->Login(Email, Password);
				return;
			}
		}
	}

	if (!Is_Valid_Email(Email))
	{
		LoginEmailErrorText->SetText(FText::FromString("*Please enter a valid Email."));
		LoginEmailEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);
		LoginEmailErrorImage->SetVisibility(ESlateVisibility::Visible);
		this->EnableLoginWidget();
		return;
	}
	if (Password == "")
	{
		LoginPasswordErrorText->SetText(FText::FromString("*Please enter your password."));
		LoginPasswordEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);
		LoginPasswordErrorImage->SetVisibility(ESlateVisibility::Visible);
		this->EnableLoginWidget();
		return;
	}
}

void ULoginMenuWidget::OnForgotPasswordButtonClicked()
{
	FString Path = "/Game/BaseCore/Textures/Login/LoginInputDefault";;
	UObject* TextureObject = LoadObject<UTexture2D>(NULL, *Path, NULL, LOAD_None, NULL);
	FSlateBrush Brush;
	Brush.SetResourceObject(TextureObject);
	ForgotPasswordEmailErrorText->SetText(FText::FromString(""));
	ForgotPasswordEmailEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);
	MenuSwitcher->SetActiveWidget(ForgotPasswordCanvas);
}

void ULoginMenuWidget::OnForgotPasswordButtonHovered()
{
	if (ForgotPasswordButtonText != nullptr)
	{
		ForgotPasswordButtonText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f,1.0f,1.0f,1.0f)));
	}
}

void ULoginMenuWidget::OnForgotPasswordButtonUnhovered()
{
	if (ForgotPasswordButtonText != nullptr)
	{
		ForgotPasswordButtonText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.75f)));
	}
}

void ULoginMenuWidget::OnSendEmailButtonClicked()
{
	if (ForgotPasswordEmailEditableTextBox == nullptr) return;

	FString Email = ForgotPasswordEmailEditableTextBox->GetText().ToString();

	FString Path = "/Game/BaseCore/Textures/Login/LoginInputError";;
	UObject* TextureObject = LoadObject<UTexture2D>(NULL, *Path, NULL, LOAD_None, NULL);
	FSlateBrush Brush;
	Brush.SetResourceObject(TextureObject);

	if (Is_Valid_Email(Email))
	{
		if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
		{
			TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

			if (TessClient != nullptr)
			{
				TesserconAPI::FOnForgotPasswordForUserResponseDelegate ForgotpassWordCallBack;
				ForgotpassWordCallBack.BindLambda([&](ETesserconError Error, FString ErrorMessage){
					if (Error == ETesserconError::EmailError)
					{
						ForgotPasswordEmailErrorText->SetText(FText::FromString("*We don't recognize that email."));
						ForgotPasswordEmailEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);
						ForgotPasswordEmailErrorText->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						ForgotPasswordEmailErrorText->SetText(FText::FromString(""));
						MenuSwitcher->SetActiveWidget(EmailSentCanvas);
					}
					});
				TessClient->Getsession().ForgotPassword(Email, ForgotpassWordCallBack);
				
				return;
			}
		}
	}
	else
	{
		ForgotPasswordEmailErrorText->SetText(FText::FromString("*Please enter a valid Email."));
		ForgotPasswordEmailEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);
		ForgotPasswordEmailErrorText->SetVisibility(ESlateVisibility::Visible);
	}
}

void ULoginMenuWidget::OnBackButtonClicked()
{
	this->DisableLoginWidget();
	this->EnableLoginWidget();
	MenuSwitcher->SetActiveWidget(LoginCanvas);
}

void ULoginMenuWidget::OnBackButtonHovered()
{
	if (BackButtonText != nullptr)
	{
		BackButtonText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)));
	}
}

void ULoginMenuWidget::OnBackButtonUnhovered()
{
	if (BackButtonText != nullptr)
	{
		BackButtonText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.75f)));
	}
}

void ULoginMenuWidget::OnBackToLoginButtonClicked()
{
	this->DisableLoginWidget();
	this->EnableLoginWidget();
	MenuSwitcher->SetActiveWidget(LoginCanvas);
}

void ULoginMenuWidget::OnLoginResponse(ETesserconError Status)
{
	FString Path = "/Game/BaseCore/Textures/Login/LoginInputError";
	UObject* TextureObject = LoadObject<UTexture2D>(NULL, *Path, NULL, LOAD_None, NULL);
	FSlateBrush Brush;
	Brush.SetResourceObject(TextureObject);
	if (Status != ETesserconError::Success)
	{
		LoginEmailErrorText->SetText(FText::FromString("*Invalid Username or Password! Try again"));
		LoginEmailEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);
		LoginPasswordEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);
		LoginEmailErrorImage->SetVisibility(ESlateVisibility::Visible);
		LoginPasswordErrorImage->SetVisibility(ESlateVisibility::Visible);
		this->EnableLoginWidget();
		return;
	}
	
	if (Status == ETesserconError::Success)
	{
		LoginEmailErrorText->SetText(FText::FromString("*Login Successfull..."));
		this->RemoveFromParent();
		UTesserconBlueprintFunctionLib::PlayLoadingScreen(true, 5.0);
	}
}

void ULoginMenuWidget::EnableLoginWidget()
{
	LoginEmailEditableTextBox->SetIsEnabled(true);
	LoginPasswordEditableTextBox->SetIsEnabled(true);
	LoginButton->SetIsEnabled(true);
	CloseButton->SetIsEnabled(true);
	MinimizeButton->SetIsEnabled(true);
	LoginButtonText->SetText(FText::FromString("Login"));
	LoginThrobberImage->SetVisibility(ESlateVisibility::Hidden);
}

void ULoginMenuWidget::DisableLoginWidget()
{
	FString Path = "/Game/BaseCore/Textures/Login/LoginInputDefault";
	UObject* TextureObject = LoadObject<UTexture2D>(NULL, *Path, NULL, LOAD_None, NULL);
	FSlateBrush Brush;
	Brush.SetResourceObject(TextureObject);

	LoginEmailEditableTextBox->SetIsEnabled(false);
	LoginEmailEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);

	LoginPasswordEditableTextBox->SetIsEnabled(false);
	LoginPasswordEditableTextBox->WidgetStyle.SetBackgroundImageNormal(Brush);

	LoginButton->SetIsEnabled(false);
	CloseButton->SetIsEnabled(false);
	MinimizeButton->SetIsEnabled(false);
	LoginEmailErrorText->SetText(FText::FromString(""));
	LoginPasswordErrorText->SetText(FText::FromString(""));
	LoginButtonText->SetText(FText::FromString(""));
	LoginEmailErrorImage->SetVisibility(ESlateVisibility::Hidden);
	LoginPasswordErrorImage->SetVisibility(ESlateVisibility::Hidden);
	LoginThrobberImage->SetVisibility(ESlateVisibility::Visible);
}