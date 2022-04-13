// Fill out your copyright notice in the Description page of Project Settings.


#include "DatabaseModel.h"

TMap<ESQLiteDataType, FString>& GetTypeMap()
{
	static TMap<ESQLiteDataType, FString> DataMap;

	if (DataMap.Num() == 0)
	{
		DataMap.Add(ESQLiteDataType::INTEGER, TEXT("INTEGER"));

		DataMap.Add(ESQLiteDataType::REAL, TEXT("REAL"));
		
		DataMap.Add(ESQLiteDataType::TEXT, TEXT("TEXT"));
		
		DataMap.Add(ESQLiteDataType::BLOB, TEXT("BLOB"));
	}
	return DataMap;
}

TMap<ESQLiteConstraint, FString>& GetConstraintMap()
{
	static TMap<ESQLiteConstraint, FString> DataMap;
	
	if (DataMap.Num() == 0)
	{
		DataMap.Add(ESQLiteConstraint::PRIMARY_KEY, TEXT("PRIMARY KEY"));
		
		DataMap.Add(ESQLiteConstraint::FORIEGN_KEY, TEXT("FORIEGN KEY"));
		
		DataMap.Add(ESQLiteConstraint::UNIQUE, TEXT("UNIQUE"));
		
		DataMap.Add(ESQLiteConstraint::NOT_NULL, TEXT("NOT NULL"));

		DataMap.Add(ESQLiteConstraint::DEFAULT, TEXT("DEFAULT"));
		
		DataMap.Add(ESQLiteConstraint::CHECK, TEXT("CHECK"));
	}
	return DataMap;
}

void DatabaseModel::RegisterMember(const TPair<TPair<FString, FString>, FString>& Pair)
{
	m_TableMap.Add(Pair.Key, Pair.Value);
}
