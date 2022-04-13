// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "TesserconCore.h"
#include "TesserconCoreCommon.h"

#include "TesserconGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class TESSERCON_API UTesserconGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	TesserconClient* m_TessClient;

	void ShowLoadingScreen();

public:
	UTesserconGameInstance(const FObjectInitializer& ObjectInitializer);
	~UTesserconGameInstance();

	FString GetAccessToken() const
	{ 
		return m_AccessToken; 
	}
	
	FString GetRefreshToken() const
	{ 
		return m_RefreshToken; 
	}
	
	FString GetLoggedInPlayerEmail() const
	{ 
		return m_LoggedInPlayerEmail; 
	}
	
	FString GetLoggedInPlayerId() const
	{ 
		return m_LoggedInPlayerId;
	}

	const FTesserconEvent& GetEvent() const
	{
		return m_Event;
	}

	void SetLoggedInPlayerEmail(const FString& Email)
	{
		m_LoggedInPlayerEmail = Email;
	}
	
	void SetAccsessTokenNull() 
	{ 
		m_AccessToken = NULL; 
	}

	void SetEvent(const FTesserconEvent& InEvent)
	{
		m_Event = InEvent;
	}

	void SetAuthenticationTokens(const FString& InAccessToken, const FString& InRefreshToken, int32 InTokenExpiresIn);
	
	virtual void SetLoggedInPlayerId(const FString& ID);

	void LoadMenu();

	void LoadLevel(const FString& LevelName);

protected:

	virtual void Init() override;
	virtual void Shutdown() override;

private:
	FString m_AccessToken;
	FString m_RefreshToken;
	int32 m_TokenExpiresIn;
	FString m_LoggedInPlayerEmail;
	FString m_LoggedInPlayerId;
	float m_VolumeValue = 0.5;
	FTesserconEvent m_Event;
};
