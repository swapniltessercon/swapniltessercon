// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/InGameGameMode.h"
#include "UI/HUD/InGameHUD.h"
#include "PlayerController/InGamePlayerController.h"

AInGameGameMode::AInGameGameMode()
{
	HUDClass = AInGameHUD::StaticClass();
	PlayerControllerClass = AInGamePlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BaseCore/Blueprints/Character/TesserconCharacter_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AInGameGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("AInGameMenuGameMode::BeginPlay()"));
}

void AInGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	FString GameMode;
	FString OnlineSessionId;

	AInGamePlayerController* VivoxPlayerController = CastChecked<AInGamePlayerController>(NewPlayer);
	VivoxPlayerController->ClientJoinVoice(GameMode, OnlineSessionId);

}

void AInGameGameMode::Logout(AController* Exiting)
{
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		TessClient->Getsession().LeaveVivoxChannels(EVivoxChannelKey::NearByChannel);
	}
	Super::Logout(Exiting);
	
}