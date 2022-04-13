#pragma once
#include "CoreMinimal.h"
#include "MatrixCommon.h"

//namespace MatrixCore
//{
//	
//}

struct MatrixLoginResponse
{
	FString UserID;
	FString AccessToken;
	FString HomeServer;
	FString DeviceID;

};

struct MatrixSyncResponse
{
	FString NextBatch;
	FMatrixRooms Rooms;
	TMap<FString, EMatrixPresence> PresenceMap;
	FMatrixAccountData AccountData;

	// TODO: future for device management and encryption
	// ToDevice
	// DeviceLists
	// One-time Keys count
};

struct MatrixRoomMembersResponse
{
	FString RoomID;
	TArray<FString> UserIDs;
	EMatrixMembership Membership;
};

struct MatrixGetMessagesResponse
{
	FString StartToken;
	FString EndToken;
	FMatrixJoinedRoomEvents RoomEvents;
	FString RoomID;
};