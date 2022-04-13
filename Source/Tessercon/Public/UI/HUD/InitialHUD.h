// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "UI/MenuWidget.h"
#include "TesserconCore.h"

#include "InitialHUD.generated.h"

/**
 * 
 */

UENUM()
enum class EWidgetType : uint8
{
	LoginMenu,
	EmptyEventList,
	EventList,
	MainMenu,
	LoggedOutMenu
};

UCLASS()
class TESSERCON_API AInitialHUD : public AHUD
{
	GENERATED_BODY()

public:
	AInitialHUD();
	void SwitchWidget(EWidgetType WidgetType);
	
	UMenuWidget* GetCurrentWidget() { return m_CurrentWidget; }

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:

	TSubclassOf<UUserWidget> m_LoginMenuClass;
	TSubclassOf<UUserWidget> m_EmptyEventMenuClass;
	TSubclassOf<UUserWidget> m_EventMenuClass;


	UMenuWidget* m_CurrentWidget;
	EWidgetType m_CurrentWidgetState;

	void OnLoginStateChanged(ETesserconLoginState LoginState);
};
