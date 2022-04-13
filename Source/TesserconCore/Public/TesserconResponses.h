#pragma once
#include "CoreMinimal.h"
#include "TesserconCoreCommon.h"

struct FTesserconLoginResponse
{
	FString UserEmail;
	FString Password;
	FTesserconTokens Tokens;
};