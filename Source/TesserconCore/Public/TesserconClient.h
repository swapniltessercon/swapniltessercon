#pragma once

#include "TesserconAPI.h"
#include "TesserconSession.h"


class TESSERCONCORE_API TesserconClient : public FTickableGameObject
{

public:

	DECLARE_DELEGATE_TwoParams(FLoginResponseDelegate, ETesserconError, FString);
	FLoginResponseDelegate d_OnLoginResponseDelegate;

	static TesserconClient* Get();

	~TesserconClient();

	void Initialize();

	void Login(const FString& Username, const FString& Password);
	
	TesserconSession& Getsession();

private:
	
	TSharedPtr<TesserconSession> m_Session;
	bool bInitialized = false;

	TesserconClient();
	TesserconClient(const TesserconClient&) = delete;

	/** Begin FTickableGameObject overrides */
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const;
	virtual bool IsTickableInEditor() const { return true; }
	/** End FTickableGameObject overrides */

	//void OnLoginResponseReceived(ETesserconError Error, FTesserconLoginResponse Response, FString ErrorMessage);
};