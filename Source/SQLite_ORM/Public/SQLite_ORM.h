// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "../ThirdParty/SQLite/Public/sqlite3.h"

#include <string>

namespace SQLite_ORM
{
    //Function returns Name of Class
    template<class C>
    FString getClassName();

    SQLITE_ORM_API FString ToString(int X);
    SQLITE_ORM_API FString ToString(int64 X);
    SQLITE_ORM_API FString ToString(float X);
    SQLITE_ORM_API FString ToString(double X);
    SQLITE_ORM_API FString ToString(const std::string& X);
    SQLITE_ORM_API FString ToString(const FString& X);
    SQLITE_ORM_API FString ToString(const FDateTime& X);
}



//Argument List: Member to be Mapped, Type of Data, Column Constraint
#define REGISTER_MEMBER(Column, Type, Constraint) RegisterMember(MakePair(#Column, Column, Type, Constraint))

#define EXPORT_CLASS(ClName)					\
template<>										\
FString SQLite_ORM::getClassName<ClName>()	    \
{	FString temp(#ClName);  return temp;}