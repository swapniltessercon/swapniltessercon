// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InGameHUD.h"
#include "UI/InGameMenu/InGameMenuWidget.h"

#include "UObject/ConstructorHelpers.h"

AInGameHUD::AInGameHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/InGameMenu/InGameMenu_WBP"));
	if (InGameMenuBPObj.Class != nullptr)
	{
		MenuClass = InGameMenuBPObj.Class;
	}
}

void AInGameHUD::SetMap(float InZoom, float InDimensions, UTexture2D* InImage)
{
	m_MapZoom = InZoom;
	m_MapDimension = InDimensions;
	m_MapImage = InImage;
}

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		UInGameMenuWidget* MenuWidget = Cast<UInGameMenuWidget>(Menu);
		TessClient->Getsession().d_EvenJoinVivoxNearByStatusResponse.AddUObject(MenuWidget, &UInGameMenuWidget::LoadChannelSession);
	}
}
