// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SQLiteDefs.h"

/**
 * 
 */

 //Base Model class for objects to be stored in DB
class SQLITE_ORM_API DatabaseModel
{
public:
	virtual ~DatabaseModel()
	{	}
protected:

	TMap<TPair<FString, FString>, FString> m_TableMap;

	void RegisterMember(const TPair<TPair<FString, FString>, FString>& Pair);

private:

	//Register Members to be exported to Database
	virtual void ExportMembers() = 0;

	//Import Members form char** in serial as they are exported to database
	virtual void ImportMembers(char** Vals) = 0;
	friend class Database;
};
