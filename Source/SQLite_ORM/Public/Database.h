// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DatabaseModel.h"

/**
 * 
 */

//Singleton class to interact with DB
class SQLITE_ORM_API Database
{
public:

	//Returns static instance 
	static Database* GetDatabaseInstance();

	virtual ~Database();

	//Creates table schema of class in DB
	template<class C>
	ESQLiteResult CreateTable();

	//Creates table for Singleton class objects
	template<class C>
	ESQLiteResult CreateTable(C* Obj);

	//Deletes table of class
	template<class C>
	ESQLiteResult DeleteTable();

	//Cleares out the data of the class table
	template<class C>
	ESQLiteResult ClearTable();

	//Inserts the data of the object passed in class table
	template<class C>
	ESQLiteResult Write(C* Obj);

	//Inserts multiple objects data in class table
	template<class C>
	ESQLiteResult WriteAll(TArray<C*> Array);

	//Updates records with objects current data
	template<class C>
	ESQLiteResult Update(C* Obj, const FString& SearchCondition);

	//Reads Database and returns pointer to newly created object of the class
	template<class C>
	C* Read(const FString& SearchCondition);

	//Reads Database and updates values of Object passed. Use this for Singleton classes
	template<class C>
	C* Read(C* Obj, const FString& SearchCondition);

	//Reads multiple objects from Database and returns array of Objects 
	template<class C>
	TArray<C*> ReadAll(const FString& SearchCondition);

	//Deletes class table from Database
	template<class C>
	ESQLiteResult Delete(const FString& SearchCondition);

private:

	sqlite3* m_DB = nullptr;

	Database();

	Database(const Database&);

	const Database operator=(const Database&);

	//Opens Database and assigns its pointer to m_DB
	void Open(const FString& FileName);

	//closes Database
	void Close();

	//Internal implementation of CreateTable
	int int_CreateTable(const FString& Table, DatabaseModel* Obj);

	//Internal implementation of DeleteTable
	int int_DeleteTable(const FString& Table);

	//Internal implementation of clear Table
	int int_ClearTable(const FString& Table);

	//Internal implementation of Write function
	int int_Write(const FString& Table, DatabaseModel* Obj);

	//Internal implementation of writeall function
	int int_WriteAll(const FString& Table, TArray<DatabaseModel*> Array);

	//Internal implementation of Update function
	int int_Update(const FString& Table, DatabaseModel* Obj, const FString& SearchCondition);

	//Internal implementation of Read function
	int int_Read(const FString& Table, DatabaseModel* Obj, const FString& SearchCondition);

	//Internal implementation of ReadAll function
	int int_ReadAll(const FString& Table, TArray<DatabaseModel*> Array, const FString& SearchCondition);

	//Internal implementation of Delete function
	int int_Delete(const FString& Table, const FString& SearchCondition);

	//Returns count of rows satisfying Search Condition
	int getCount(const FString& Table, const FString& SearchCondition);

	//Call back function for getCount
	static int CountCallback(void* Obj, int argc, char** argv, char** azColName);

	//Callback function for Read function
	static int ReadCallback(void* Obj, int argc, char** argv, char** azColName);
};


template<class C>
ESQLiteResult Database::CreateTable()
{
	FString Table = SQLite_ORM::getClassName<C>();

	DatabaseModel* Object = dynamic_cast<DatabaseModel*>(new C());

	int result = int_CreateTable(Table, Object);

	return ESQLiteResult(result);
}

template<class C>
inline ESQLiteResult Database::CreateTable(C* Obj)
{
	FString Table = SQLite_ORM::getClassName<C>();

	DatabaseModel* Object = dynamic_cast<DatabaseModel*>(Obj);

	int result = int_CreateTable(Table, Object);

	return ESQLiteResult(result);
}

template<class C>
ESQLiteResult Database::DeleteTable()
{
	FString Table = SQLite_ORM::getClassName<C>();

	int result = this->int_DeleteTable(Table);

	return ESQLiteResult(result);
}

template<class C>
ESQLiteResult Database::ClearTable()
{
	FString Table = SQLite_ORM::getClassName<C>();

	int result = this->int_ClearTable(Table);

	return ESQLiteResult(result);
}

template<class C>
ESQLiteResult Database::Write(C* Obj)
{
	FString Table = SQLite_ORM::getClassName<C>();

	int result = int_Write(Table, Obj);

	return ESQLiteResult(result);
}

template<class C>
ESQLiteResult Database::WriteAll(TArray<C*> Array)
{
	FString Table = SQLite_ORM::getClassName<C>();

	TArray<DatabaseModel*> ModelArray;

	for (int i = 0; i < Array.Num(); i++)
	{
		ModelArray.Add(dynamic_cast<DatabaseModel*>(Array[i]));
	}

	int result = this->int_WriteAll(Table, ModelArray);

	return ESQLiteResult(result);
}

template<class C>
ESQLiteResult Database::Update(C* Obj, const FString& SearchCondition)
{
	FString Table = SQLite_ORM::getClassName<C>();

	int result = this->int_Update(Table, Obj, SearchCondition);

	return ESQLiteResult(result);
}

template<class C>
C* Database::Read(const FString& SearchCondition)
{
	FString Table = SQLite_ORM::getClassName<C>();

	C* Obj = new C();

	DatabaseModel* ObjPtr = dynamic_cast<DatabaseModel*>(Obj);

	int result = int_Read(Table, ObjPtr, SearchCondition);

	return Obj;
}

template<class C>
inline C* Database::Read(C* Obj, const FString& SearchCondition)
{
	FString Table = SQLite_ORM::getClassName<C>();

	DatabaseModel* ObjPtr = dynamic_cast<DatabaseModel*>(Obj);

	int result = int_Read(Table, ObjPtr, SearchCondition);

	return Obj;
}

template<class C>
TArray<C*> Database::ReadAll(const FString& SearchCondition)
{
	FString Table = SQLite_ORM::getClassName<C>();

	int ObjectCount = getCount(Table, SearchCondition);

	TArray<C*> Array;

	TArray<DatabaseModel*> ModelArray;

	for (int i = 0; i < ObjectCount; i++)
	{
		C* Ptr = new C();

		DatabaseModel* ModelPtr = dynamic_cast<DatabaseModel*>(Ptr);

		Array.Add(Ptr);

		ModelArray.Add(ModelPtr);
	}

	int result = int_ReadAll(Table, ModelArray, SearchCondition);

	return Array;
}

template<class C>
ESQLiteResult Database::Delete(const FString& SearchCondition)
{
	FString Table = SQLite_ORM::getClassName<C>();

	int result = int_Delete(Table, SearchCondition);

	return ESQLiteResult(result);
}