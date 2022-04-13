#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "TesserconClient.h"

class TESSERCONCORE_API FTesserconCoreModule : public IModuleInterface
{
public:
	virtual void ShutdownModule() override;
	virtual void StartupModule() override;

	TesserconClient* GetTesserconClient();
};