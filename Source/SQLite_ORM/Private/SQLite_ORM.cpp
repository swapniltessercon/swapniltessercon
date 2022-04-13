// Copyright Epic Games, Inc. All Rights Reserved.

#include "SQLite_ORM.h"

#include "Modules/ModuleManager.h"
#include "..\Public\SQLite_ORM.h"

IMPLEMENT_MODULE( FDefaultGameModuleImpl, SQLite_ORM);

FString SQLite_ORM::ToString(int X)
{
	return FString::FromInt(X);
}

FString SQLite_ORM::ToString(int64 X)
{
	return FString::FromInt(X);
}

FString SQLite_ORM::ToString(float X)
{
	return FString::SanitizeFloat(X);
}

FString SQLite_ORM::ToString(double X)
{
	return FString::SanitizeFloat(X);
}

FString SQLite_ORM::ToString(const std::string& X)
{
	return FString(X.c_str());
}

FString SQLite_ORM::ToString(const FString& X)
{
	return X;
}

FString SQLite_ORM::ToString(const FDateTime& X)
{
	return X.ToString();
}

