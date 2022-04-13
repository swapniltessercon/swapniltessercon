// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/BaseHUD.h"



void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) return;

	if (MenuClass != nullptr)
	{
		Menu = CreateWidget<UBaseMenuWidget>(GetWorld(), MenuClass);
		if (Menu != nullptr)
		{
			Menu->AddToViewport(-1);
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(Menu->TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->bShowMouseCursor = true;
		}
	}

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		if (TessClient != nullptr)
		{
			TessClient->Getsession().d_EventOnLoginStateChanged.AddUObject(this, &ABaseHUD::OnLoginStateChanged);
		}
	}
}

void ABaseHUD::OnLoginStateChanged(ETesserconLoginState LoginState)
{
	if (LoginState == ETesserconLoginState::LoggedOut)
	{
		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;

		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (!ensure(PlayerController != nullptr)) return;

		FString TravelURL = "/Game/BaseCore/Maps/Login/LoginLevel";

		PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}
}