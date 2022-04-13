// Fill out your copyright notice in the Description page of Project Settings.


#include "TesserconBlueprintFunctionLib.h"
#include "TesserconLoadingScreen.h"



void UTesserconBlueprintFunctionLib::PlayLoadingScreen(bool bPlayUntilStopped, float PlayTime)
{
	ITesserconLoadingScreenModule& LoadingScreenModule = ITesserconLoadingScreenModule::Get();
	LoadingScreenModule.StartInGameLoadingScreen(bPlayUntilStopped, PlayTime);
}

void UTesserconBlueprintFunctionLib::StopLoadingScreen()
{
	ITesserconLoadingScreenModule& LoadingScreenModule = ITesserconLoadingScreenModule::Get();
	LoadingScreenModule.StopInGameLoadingScreen();
}