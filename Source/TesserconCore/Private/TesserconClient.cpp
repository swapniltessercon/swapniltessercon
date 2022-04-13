// Fill out your copyright notice in the Description page of Project Settings.
#include "TesserconClient.h"


TesserconClient::TesserconClient()
{
	
}

TesserconClient::~TesserconClient()
{

}

TesserconClient* TesserconClient::Get()
{
	static TesserconClient Instance;
	return &Instance;
}



void TesserconClient::Initialize()
{
	m_Session = TSharedPtr<TesserconSession>(new TesserconSession());
	m_Session->Initialize();
	bInitialized = true;
}

/** Begin FTickableGameObject overrides */
TStatId TesserconClient::GetStatId() const
{
	return TStatId();
}

void TesserconClient::Tick(float DeltaTime)
{


}
/** End FTickableGameObject overrides */


TesserconSession& TesserconClient::Getsession()
{
	if (!bInitialized)
		Initialize();

	return *m_Session;
}


void TesserconClient::Login(const FString& Username, const FString& Password)
{
	if (m_Session != nullptr)
	{
		m_Session->Login(Username, Password);
	}	
}
