// Fill out your copyright notice in the Description page of Project Settings.


#include "TesserconGameInstance.h"

#include "Misc/FileHelper.h"

#include "Engine/SkeletalMesh.h"

# if PLATFORM_ANDROID
#include "Misc/App.h"
#endif

UTesserconGameInstance::UTesserconGameInstance(const FObjectInitializer& ObjectInitializer)
{
    //TODO
}

UTesserconGameInstance::~UTesserconGameInstance()
{
    //TODO
}

void UTesserconGameInstance::Init()
{
    Super::Init();

    m_TessClient = static_cast<FTesserconCoreModule*>(&FModuleManager::Get().LoadModuleChecked(TEXT("TesserconCore")))->GetTesserconClient();

    if (m_TessClient != nullptr)
    {
        m_TessClient->Initialize();
    }
}

void UTesserconGameInstance::Shutdown()
{
    if (m_TessClient != nullptr)
    {
        m_TessClient->~TesserconClient();
    }

    FModuleManager::Get().UnloadModule(TEXT("TesserconCore"));

    Super::Shutdown();
}

void UTesserconGameInstance::SetAuthenticationTokens(const FString& InAccessToken, const FString& InRefreshToken, int32 InTokenExpiresIn)
{
    m_TokenExpiresIn = InTokenExpiresIn;
    m_AccessToken = InAccessToken;
    m_RefreshToken = InRefreshToken;
}

void UTesserconGameInstance::SetLoggedInPlayerId(const FString& ID)
{
    m_LoggedInPlayerId = ID;
}

void UTesserconGameInstance::LoadMenu()
{
    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (!ensure(PlayerController != nullptr)) return;
}


void UTesserconGameInstance::LoadLevel(const FString& LevelName)
{
    FString Level = LevelName + "?Listen";
    UWorld* World = GetWorld();
    if (!ensure(World != nullptr)) return;
    World->ServerTravel(Level);
}

void UTesserconGameInstance::ShowLoadingScreen()
{

}