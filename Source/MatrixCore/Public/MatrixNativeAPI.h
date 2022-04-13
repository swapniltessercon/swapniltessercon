#pragma once

#include "CoreMinimal.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "MatrixCommon.h"
#include "MatrixResponses.h"

class MatrixNativeAPI
{

public:

	DECLARE_DELEGATE_OneParam(FOnMatrixAPIResponseDelegate, EMatrixCoreError);

	DECLARE_DELEGATE_TwoParams(FOnMatrixRoomMembersResponseDelegate, EMatrixCoreError, MatrixRoomMembersResponse);

	// Error, RoomID of created room
	DECLARE_DELEGATE_TwoParams(FOnMatrixCreateRoomReponseDelegate, EMatrixCoreError, FString);

	DECLARE_DELEGATE_ThreeParams(FDelegateMatrixLoginResponse, EMatrixCoreError, MatrixLoginResponse, FString);
	FDelegateMatrixLoginResponse d_OnLoginResponseReceived;

	// TODO: not needed remove it later
	DECLARE_DELEGATE_TwoParams(FDelegateMatrixLogoutResponse, EMatrixCoreError, FString);
	FDelegateMatrixLogoutResponse d_OnLogoutResponseReceived;

	DECLARE_DELEGATE_FourParams(FDelegateMatrixSyncResponse, EMatrixCoreError, MatrixSyncResponse, bool, FString);
	FDelegateMatrixSyncResponse d_OnSyncResponseReceived;

	DECLARE_DELEGATE_FiveParams(FDelegateMatrixCreateRoomResponse, EMatrixCoreError, FString, bool, FString, FString);
	FDelegateMatrixCreateRoomResponse d_OnCreateRoomResponseReceived;

	DECLARE_DELEGATE_ThreeParams(FDelegateMatrixJoinRoomResponse, EMatrixCoreError, FString, FString)
	FDelegateMatrixJoinRoomResponse d_OnJoinRoomResponseReceived;

	DECLARE_DELEGATE_ThreeParams(FDelegateMatrixLeaveRoomResponse, EMatrixCoreError, FString, FString)
	FDelegateMatrixLeaveRoomResponse d_OnLeaveRoomResponseReceived;

	DECLARE_DELEGATE_ThreeParams(FDelegateMatrixRoomMembersResponse, EMatrixCoreError, MatrixRoomMembersResponse, FString)
	FDelegateMatrixRoomMembersResponse d_OnRoomMembersResponseReceived;

	DECLARE_DELEGATE_ThreeParams(FDelegateMatrixGetMessagesResponse, EMatrixCoreError, MatrixGetMessagesResponse, FString)
	FDelegateMatrixGetMessagesResponse d_OnGetMessagesResponseReceived;

	void Login(const FString& Username, const FString& Password);
	void Logout(const FString& AccessToken);

	void Sync(const FString& AccessToken, const FString& NextBatchToken, bool bIsInitial = true);

	void CreateRoom(const FString& AccessToken, const TArray<FString>& UserIDs, bool bIsDirect, const FString& RoomName, const FOnMatrixCreateRoomReponseDelegate& TheDelegate = FOnMatrixCreateRoomReponseDelegate());
	void InviteToRoom(const FString& AccessToken, const FString& UserID, const FString& RoomID);
	void JoinRoom(const FString& AccessToken, const FString& RoomID, const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());
	void LeaveRoom(const FString& AccessToken, const FString& RoomID, const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());

	// not required at this point
	// void ForgetRoom(FString AccessToken, FString RoomID);\

	void UpdateDirectConnections(const FString& AccessToken, const TMap<FString, FString>& UserRoomPair, const FString& UserID, const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());

	// Room Specific API's
	void GetRoomMembers(const FString& AccessToken, const FString& RoomID, EMatrixMembership Membership, const FOnMatrixRoomMembersResponseDelegate& TheDelegate = FOnMatrixRoomMembersResponseDelegate());

	void SendMessage(const FString& AccessToken, const FString& RoomID, const FString& Text, const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());

	void GetMessages(const FString& AccessToken, const FString& RoomID, const FString& FromToken, bool bIsBackward = true, const FString& ToToken = "", int limit = 10);

	void SendCallInvite(const FString& AccessToken, const FString& RoomID, const FString& CallID, const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());
	void SendCallAnswer(const FString& AccessToken, const FString& RoomID, const FString& CallID, const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());
	void SendCallHangup(const FString& AccessToken, const FString& RoomID, const FString& CallID, EMatrixCallHangUpReason Reason = EMatrixCallHangUpReason::None, 
		const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());

	~MatrixNativeAPI();

	MatrixNativeAPI();

	// TODO: check if the default argument is needed
	void UpdateIgnoreList(const FString& AccessToken, const TArray<FString>& BlockedUsers, const FString& UserID, const FOnMatrixAPIResponseDelegate& TheDelegate);

	void UpdateRoomName(const FString& AccessToken, const FString& RoomID, const FString& NewName, const FOnMatrixAPIResponseDelegate& TheDelegate);

	void KickUserFromRoom(const FString& AccessToken, const FString& RoomID, const FString& UserID, const FString& Reason = "",
		const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());

	void SendTypingNotificationForRoom(const FString& AccessToken, const FString& RoomID, const FString& UserID, bool bIstyping, int32 Millisec = 5000,
		const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());

	void UpdateFullyReadMarkerForRoom(const FString& AccessToken, const FString& RoomID, const FString& FullyReadEventID, const FString& ReadEventID = "",
		const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());

	void UpdateMembershipStateManuallyToLeave(const FString& AccessToken, const FString& RoomID, const FString& UserID, const FOnMatrixAPIResponseDelegate& TheDelegate = FOnMatrixAPIResponseDelegate());


private:

	FHttpModule* p_HttpModule;

	void OnMatrixLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void OnMatrixLogoutResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnMatrixSyncResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void OnMatrixCreateRoomResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnMatrixCreateRoomReponseDelegate TheDelegate = FOnMatrixCreateRoomReponseDelegate());
	void OnMatrixJoinRoomResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnMatrixAPIResponseDelegate TheDelegate);
	void OnMatrixLeaveRoomResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnMatrixAPIResponseDelegate TheDelegate);

	//void OnMatrixUpdateDirectConnections(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Room Specific
	void OnMatrixRoomMembersResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnMatrixRoomMembersResponseDelegate TheDelegate);

	void OnMatrixGetMessagesResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FString GetRequestContentAsString(FHttpRequestPtr Request);

	FMatrixJoinedRoomEvents ProcessEventsForJoinedRoom(const TArray<TSharedPtr<FJsonValue>>& Events);
};



