#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "MatrixClient.h"
#include "MatrixResponses.h"


class MATRIXCORE_API FMatrixCoreModule : public IModuleInterface
{
public:
	virtual void ShutdownModule() override;
	virtual void StartupModule() override;

	MatrixClient* GetMatrixClient() const;
};