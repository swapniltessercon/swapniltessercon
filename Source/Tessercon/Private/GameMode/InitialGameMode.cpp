// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/InitialGameMode.h"
#include "UI/HUD/InitialHUD.h"

AInitialGameMode::AInitialGameMode()
{
	HUDClass = AInitialHUD::StaticClass();
}