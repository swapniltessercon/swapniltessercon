// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/InGamePlayerController.h"
#include "TesserconGameInstance.h"


AInGamePlayerController::AInGamePlayerController()
{
}
void AInGamePlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void AInGamePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

void AInGamePlayerController::ClientJoinVoice_Implementation(const FString& GameMode, const FString& OnlineSessionId, const int32& TeamNum)
{
 
    TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
    if (TessClient != nullptr)
    {  
        TessClient->Getsession().JoinVivoxChannel("TesserconNearBy", EVivoxChannelKey::NearByChannel/*, InnerDelegate*/);
    }
}

void AInGamePlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
    if (TessClient != nullptr)
    {
        TessClient->Getsession().Update3DPosition(GetPawnOrSpectator()); // Track player in either Warmup or Match.
    }
}

