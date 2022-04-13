// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/MainMenuHUD.h"

#include "UObject/ConstructorHelpers.h"

AMainMenuHUD::AMainMenuHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/MainMenu_WBP"));
	if (MainMenuBPObj.Class != nullptr)
	{
		MenuClass = MainMenuBPObj.Class;
	}
}

AMainMenuHUD::~AMainMenuHUD()
{
	Menu = nullptr;
}