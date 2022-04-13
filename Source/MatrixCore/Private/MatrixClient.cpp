#include "MatrixClient.h"
#include "MatrixCommon.h"

MatrixClient::MatrixClient()
{

}


MatrixClient* MatrixClient::Get()
{
	static MatrixClient Instance;
	//UE_LOG(LogMatrixCore, Warning, TEXT("MatrixClient::Get() -----------------"));

	return &Instance;
}


MatrixClient::~MatrixClient()
{
	//UE_LOG(LogMatrixCore, Warning, TEXT("MatrixClient:: Destructor -------------------"));
}

MatrixSession& MatrixClient::GetSession()
{
	if (!bInitialized)
		Initialize();
	
	return *m_Session;
}

void MatrixClient::Initialize()
{

	UE_LOG(LogMatrixCore, Warning, TEXT("MatrixClient:: Initialize()-------------------"));
	m_Session = TSharedPtr<MatrixSession>(new MatrixSession());

	m_Session->Initialize();

	bInitialized = true;
}

FString MatrixClient::GetUsernameFromDB()
{
	FString Username;
	Database* DB = Database::GetDatabaseInstance();

	if (DB != nullptr)
	{
		MatrixSession* DBSession = DB->Read<MatrixSession>("");

		FString UserID = DBSession->GetLoggedInUserID();

		FString LeftTemp, Right;

		UserID.Split("@", &LeftTemp, &Right);
		Right.Split(":", &Username, &LeftTemp);
	}

	return Username;
}

void MatrixClient::Login(const FString& Username, const FString& Password)
{
	if (m_Session != nullptr)
	{
		m_Session->~MatrixSession();
		m_Session = nullptr;
		bInitialized = false;
	}
	GetSession().Login(Username, Password);
}

void MatrixClient::Logout()
{
	/*if (m_Session != nullptr)
	{
		m_Session->Logout();
	}*/
	GetSession().Logout();
	GetSession().~MatrixSession();

	bInitialized = false;
}