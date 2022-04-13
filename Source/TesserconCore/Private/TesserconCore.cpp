#include "TesserconCore.h"

void FTesserconCoreModule::ShutdownModule()
{
	TesserconClient::Get()->~TesserconClient();
}

void FTesserconCoreModule::StartupModule()
{

}

TesserconClient* FTesserconCoreModule::GetTesserconClient()
{
	return TesserconClient::Get();
}


IMPLEMENT_MODULE(FTesserconCoreModule, TesserconCore);