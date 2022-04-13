// Fill out your copyright notice in the Description page of Project Settings.


#include "Database.h"

#include "Misc/App.h"

#include <vector>
#include <mutex>

std::mutex ThreadLock;

Database::Database()
{
	FString FileName = FApp::GetProjectName();
	this->Open(FileName);
}

Database* Database::GetDatabaseInstance()
{
	static Database Instance;

	return &Instance;
}

Database::~Database()
{
	this->Close();
}

void Database::Open(const FString& FileName)
{
	m_DB = nullptr;
	
	FString DB_filepath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + FileName + ".db");

	int rc = sqlite3_open((TCHAR_TO_UTF8(*DB_filepath)), &m_DB);
}

void Database::Close()
{
	UE_LOG(LogTemp, Warning, TEXT("[Database] Closing Database...."));
}


int Database::int_CreateTable(const FString& Table, DatabaseModel* Obj)
{
	std::lock_guard<std::mutex> Lock(ThreadLock);

	FString Query = "CREATE TABLE IF NOT EXISTS ";
	Query += Table;
	Query += " (";

	Obj->ExportMembers();

	for (auto Pair: Obj->m_TableMap)
	{
		Query += Pair.Key.Key + " ";
		Query += Pair.Key.Value + ", ";
	}

	Obj->m_TableMap.Empty();

	Query += ");";
	Query = Query.Replace(TEXT(", );"), TEXT(");"));

	Query = Query.Replace(TEXT("."), TEXT("_"));

	char* Err;

	return sqlite3_exec(m_DB, TCHAR_TO_UTF8(*Query), nullptr, 0, &Err);
}

int Database::int_DeleteTable(const FString& Table)
{
	FString Query = "DROP TABLE " + Table + ";";

	char* Err;

	return sqlite3_exec(m_DB, TCHAR_TO_UTF8(*Query), nullptr, 0, &Err);
}

int Database::int_ClearTable(const FString& Table)
{
	FString Query = "DELETE FROM " + Table + ";";

	char* Err;

	return sqlite3_exec(m_DB, TCHAR_TO_UTF8(*Query), nullptr, 0, &Err);
}

int Database::int_Write(const FString& Table, DatabaseModel* Obj)
{
	std::lock_guard<std::mutex> Lock(ThreadLock);

	FString Query = "INSERT INTO " + Table + " (";
	FString Columns = "";
	FString Values = "";

	Obj->ExportMembers();

	for (auto Pair : Obj->m_TableMap)
	{
		Columns += Pair.Key.Key + ", ";
		Values += Pair.Value + ", ";
	}

	Obj->m_TableMap.Empty();

	Query += Columns;
	Query += ")";
	Query = Query.Replace(TEXT(", )"), TEXT(") "));
	Query = Query.Replace(TEXT("."), TEXT("_"));

	Query += "VALUES (";
	Query += Values;
	Query += ");";

	Query = Query.Replace(TEXT(", );"), TEXT(");"));

	char* Err;

	return sqlite3_exec(m_DB, TCHAR_TO_UTF8(*Query), nullptr, 0, &Err);
}

int Database::int_WriteAll(const FString& Table, TArray<DatabaseModel*> Array)
{
	std::lock_guard<std::mutex> Lock(ThreadLock);

	int result=-1;
	for (int i = 0; i < Array.Num(); i++)
	{
		FString Query = "INSERT INTO " + Table + " (";
		FString Columns = "";
		FString Values = "";

		Array[i]->ExportMembers();

		for (auto Pair : Array[i]->m_TableMap)
		{
			Columns += Pair.Key.Key + ", ";
			Values += Pair.Value + ", ";
		}

		Array[i]->m_TableMap.Empty();

		Query += Columns;
		Query += ")";
		Query = Query.Replace(TEXT(", )"), TEXT(") "));
		Query = Query.Replace(TEXT("."), TEXT("_"));

		Query += "VALUES (";
		Query += Values;
		Query += ");";

		Query = Query.Replace(TEXT(", );"), TEXT(");"));

		char* Err;

		result = sqlite3_exec(m_DB, TCHAR_TO_UTF8(*Query), nullptr, 0, &Err);
	}

	return result;
}

int Database::int_Update(const FString& Table, DatabaseModel* Obj, const FString& SearchCondition)
{
	std::lock_guard<std::mutex> Lock(ThreadLock);

	FString Query = "UPDATE " + Table + " SET ";

	Obj->ExportMembers();

	for (auto Pair : Obj->m_TableMap)
	{
		Query += Pair.Key.Key.Replace(TEXT("."), TEXT("_")) + " = ";
		Query += Pair.Value + ", ";
	}

	Obj->m_TableMap.Empty();
	Query += ")";

	Query = Query.Replace(TEXT(", )"), TEXT(""));

	if (!SearchCondition.IsEmpty())
	{
		Query += " WHERE ";

		Query += SearchCondition;
	}

	char* Err;

	return sqlite3_exec(m_DB, TCHAR_TO_UTF8(*Query), nullptr, 0, &Err);
}

int Database::CountCallback(void* Obj, int argc, char** argv, char** azColName)
{
	int* Count = static_cast<int*>(Obj);

	*Count = std::atoi(argv[0]);

	return 0;
}

int Database::getCount(const FString& Table, const FString& SearchCondition)
{
	FString Query = "SELECT COUNT(*) FROM " + Table + " WHERE " + SearchCondition + ";";

	int* count = new int();

	char* Err;

	int result = sqlite3_exec(m_DB, TCHAR_TO_UTF8(*Query), CountCallback, count, &Err);

	return *count;
}

int Database::ReadCallback(void* Obj, int argc, char** argv, char** azColName)
{
	std::lock_guard<std::mutex> Lock(ThreadLock);

	DatabaseModel* Object = static_cast<DatabaseModel*>(Obj);

	Object->ImportMembers(argv);

	return 0;
}

int Database::int_Read(const FString& Table, DatabaseModel* Obj, const FString& SearchCondition)
{
	FString Query = "SELECT * FROM " + Table;
	if(!SearchCondition.IsEmpty())
		Query += " WHERE " + SearchCondition;

	char* Err;

	return sqlite3_exec(m_DB, TCHAR_TO_UTF8(*Query), ReadCallback, Obj, &Err);
}

int Database::int_ReadAll(const FString& Table, TArray<DatabaseModel*> Array, const FString& SearchCondition)
{
	std::lock_guard<std::mutex> Lock(ThreadLock);

	FString Query = "SELECT * FROM " + Table + " WHERE " + SearchCondition;

	sqlite3_stmt* Statement;

	int result = sqlite3_prepare(m_DB, TCHAR_TO_UTF8(*Query), -1, &Statement, nullptr);

	int Step = 0, i = 0;

	for (Step = sqlite3_step(Statement); Step == SQLITE_ROW && Step != SQLITE_DONE; Step = sqlite3_step(Statement), i++)
	{
		std::vector<char*> Values;

		for (int Column = 0; Column < sqlite3_column_count(Statement); Column++)
		{
			Values.push_back((char*)sqlite3_column_text(Statement, Column));
		}

		Array[i]->ImportMembers(Values.data());
	}

	return result;
}

int Database::int_Delete(const FString& Table, const FString& SearchCondition)
{
	FString Query = "DELETE FROM " + Table + " WHERE " + SearchCondition + " ;";

	char* Err;

	return sqlite3_exec(m_DB, TCHAR_TO_UTF8(*Query), nullptr, 0, &Err);
}