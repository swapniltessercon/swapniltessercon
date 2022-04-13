#pragma once
#include "MatrixNativeAPI.h"
#include "MatrixSession.h"

class MATRIXCORE_API MatrixClient
{
public:

	static MatrixClient* Get();

	~MatrixClient();

	void Initialize();

	MatrixSession& GetSession();

	FString GetUsernameFromDB();

	void Login(const FString& Username, const FString& Password);

	void Logout();

private:

	bool bInitialized = false;

	TSharedPtr<MatrixSession> m_Session;

	MatrixClient();

	MatrixClient(const MatrixClient&) = delete;

	MatrixClient& operator=(const MatrixClient&) = delete;
};

