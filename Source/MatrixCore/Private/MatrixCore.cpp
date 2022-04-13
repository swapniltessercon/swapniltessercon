#include "MatrixCore.h"
#include "MatrixCommon.h"


void FMatrixCoreModule::ShutdownModule()
{
	MatrixClient::Get()->~MatrixClient();
}

void FMatrixCoreModule::StartupModule()
{
	UE_LOG(LogMatrixCore, Warning, TEXT("MatrixCore module - On Start Module ---------------"));
}

MatrixClient* FMatrixCoreModule::GetMatrixClient() const
{
	return MatrixClient::Get();
}

IMPLEMENT_MODULE(FMatrixCoreModule, MatrixCore);
