// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InitialHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "TesserconBlueprintFunctionLib.h"
#include "Engine/GameEngine.h"
#include "EngineGlobals.h"


AInitialHUD::AInitialHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> LoginMenuBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/Login/LoginMenu_WBP"));
	if (LoginMenuBPObj.Class != nullptr)
	{
		m_LoginMenuClass = LoginMenuBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> EmptyEventMenuBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/SelectEvent/EmptySelectEventMenu_WBP"));
	if (EmptyEventMenuBPObj.Class != nullptr)
	{
		m_EmptyEventMenuClass = EmptyEventMenuBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> EventMenuBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/SelectEvent/SelectEventMenu_WBP"));
	if (EventMenuBPObj.Class != nullptr)
	{
		m_EventMenuClass = EventMenuBPObj.Class;
	}
}

void AInitialHUD::BeginPlay()
{
	Super::BeginPlay();

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		if (TessClient != nullptr)
		{
			TessClient->Getsession().d_EventOnLoginStateChanged.AddUObject(this, &AInitialHUD::OnLoginStateChanged);
			ETesserconLoginState LoginState = TessClient->Getsession().GetLoginState();

			if (LoginState == ETesserconLoginState::LoggedOut)
			{
				SwitchWidget(EWidgetType::LoginMenu);
			}
			else if (LoginState == ETesserconLoginState::LogginIn)
			{
				UE_LOG(LogTemp, Warning, TEXT("User update ETesserconLoginState ----------LoggingIn"))
				UTesserconBlueprintFunctionLib::PlayLoadingScreen(true, 5.0);
			}
			else if (LoginState == ETesserconLoginState::LoggedInNoActiveEvents)
			{
				UE_LOG(LogTemp, Warning, TEXT("User update ETesserconLoginState ----------LoggedIn"))
				SwitchWidget(EWidgetType::EmptyEventList);
			}
			else if (LoginState == ETesserconLoginState::LoggedIn)
			{
				UE_LOG(LogTemp, Warning, TEXT("User update ETesserconLoginState ----------LoggedIn"))
				SwitchWidget(EWidgetType::EventList);
			}
		}
	}
}

void AInitialHUD::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		
	}
}

void AInitialHUD::OnLoginStateChanged(ETesserconLoginState LoginState)
{
	
	if (LoginState == ETesserconLoginState::LoggedOut)
	{
		UE_LOG(LogTemp, Warning, TEXT("User update ETesserconLoginState ----------LoggedOut"));
		SwitchWidget(EWidgetType::LoginMenu);
	}
	else if (LoginState == ETesserconLoginState::LoggedInNoActiveEvents)
	{
		UE_LOG(LogTemp, Warning, TEXT("User update ETesserconLoginState ----------LoggedInNoActiveEvents"));
		SwitchWidget(EWidgetType::EmptyEventList);
	}
	else if (LoginState == ETesserconLoginState::LoggedIn)
	{
		UE_LOG(LogTemp, Warning, TEXT("User update ETesserconLoginState ----------LoggedIn"));
		SwitchWidget(EWidgetType::EventList);
	}
	/*else 
	{
		UE_LOG(LogTemp, Warning, TEXT("User update ETesserconLoginState ----------LoggingIn"));
		UTesserconBlueprintFunctionLib::PlayLoadingScreen(true, 5.0);
	}*/
}

void AInitialHUD::SwitchWidget(EWidgetType WidgetType)
{
	if (m_CurrentWidget != nullptr)
	{
		m_CurrentWidget->Teardown();
	}

	m_CurrentWidgetState = WidgetType;
	TSubclassOf<UMenuWidget> WidgetClass;
	EInputModeType InputModeType = EInputModeType::UIOnly;

	switch (WidgetType)
	{
		case EWidgetType::LoginMenu:
			WidgetClass = m_LoginMenuClass;
			InputModeType = EInputModeType::UIOnly;
			break;
		case EWidgetType::EmptyEventList:
			WidgetClass = m_EmptyEventMenuClass;
			InputModeType = EInputModeType::UIOnly;
			break;
		case EWidgetType::EventList:
			WidgetClass = m_EventMenuClass;
			InputModeType = EInputModeType::UIOnly;
			break;
		default:
			break;
	}


	if (WidgetClass != nullptr)
	{
		m_CurrentWidget = CreateWidget<UMenuWidget>(GetWorld(), WidgetClass);
		if (m_CurrentWidget == nullptr) 
			return;
		m_CurrentWidget->Setup(InputModeType);
	}
}