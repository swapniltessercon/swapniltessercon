#pragma once
#include "CoreMinimal.h"

#include "MatrixNativeAPI.h"
#include "MatrixCommon.h"
#include "RoomSession.h"

#include "Database.h"

//#include "MatrixSession.generated.h"

#include "TesserconClient.h"
//class TesserconClient;

//UENUM()

enum class ESyncState : uint8
{
	Started,
	Finished
};

class MATRIXCORE_API MatrixSession : public FTickableGameObject, public DatabaseModel
{

public:

	// TODO: change this to event for login state and add a delegate for login response

	//DECLARE_DELEGATE_TwoParams(FOnMatrixResponse, EMatrixCoreError, FString);
	//FOnMatrixResponse OnLoginResponse;

	//Changes
	DECLARE_EVENT_OneParam(MatrixSession, FLoginStateChanged, ELoginState);
	FLoginStateChanged d_EventOnLoginStateChange;

	DECLARE_EVENT(MatrixSession, FSyncResponseHasEvents);
	FSyncResponseHasEvents d_EventSyncResponse;

	// FString -> Friend ID
	// TODO: change this to one event
	DECLARE_EVENT_OneParam(MatrixSession, FOnFriendAdded, FString);
	DECLARE_EVENT_OneParam(MatrixSession, FOnFriendRemoved, FString);

	// FString -> RoomID
	DECLARE_EVENT_OneParam(MatrixSession, FOnTeamAdded, FString);
	DECLARE_EVENT_OneParam(MatrixSession, FOnTeamRemoved, FString);

	// TODO: change this to one event
	DECLARE_EVENT_OneParam(MatrixSession, FOnFriendRequestReceived, FMatrixInvitedRoom);
	DECLARE_EVENT_OneParam(MatrixSession, FOnTeamRequestReceived, FMatrixInvitedRoom);

	DECLARE_EVENT_OneParam(MatrixSession, FOnRoomInviteRequestWithdrawn, FString);

	DECLARE_EVENT_ThreeParams(MatrixSession, FOnCallEventReceived, EMatrixCallEventType, FString, FMatrixCallEvent);

	DECLARE_EVENT_OneParam(MatrixSession, FOnChatEventNotification, bool);
	

	// Event called on friend added - broadcast user_id 
	FOnFriendAdded d_EventOnFriendAdded;

	//Event called on friend removed
	FOnFriendRemoved d_EventOnFriendRemoved;

	// Event on friend request received -  broadcast FMatrixInvitedRoom
	FOnFriendRequestReceived d_EventOnFriendRequestReceived;

	// Event on Team Request received - broadcast FMatrixInvitedRoom
	FOnTeamRequestReceived d_EventOnTeamRequestReceived;

	// Event on new team added - broadcast roomid
	FOnTeamAdded d_EventOnTeamAdded;

	//// Event on team removed - broadcast roomid
	FOnTeamRemoved d_EventOnTeamRemoved;

	FOnRoomInviteRequestWithdrawn d_EventOnReceivedFriendRequestWithdrawn;

	FOnRoomInviteRequestWithdrawn d_EventOnReceivedTeamRequestWithdrawn;

	FOnChatEventNotification d_EventOnTeamChatEventNotification;
	FOnChatEventNotification d_EventOnFriendChatEventNotification;

	ELoginState GetLoginState() const
	{
		return m_LoginState;
	}

	// user login-log out
	void Login(const FString& Username, const FString& Password);
	void Logout();

	// sync

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	// TODO: change all the public functions accessed in the room session to private and use friend relation
	// ------------------------------------------------------------------------------------------------------------------------------------------------

	// create room
	void CreateRoom(const TArray<FString>& UserIds, bool bIsDirect, const FString& RoomName, 
			const MatrixNativeAPI::FOnMatrixCreateRoomReponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixCreateRoomReponseDelegate());
	//join room
	void JoinRoom(const FString& RoomID, bool bIsDirect, const FString& Sender, 
			const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());
	// leave room
	void LeaveRoom(const FString& RoomID, bool bIsDirect, 
		const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());
	// invite user to room
	void InviteUserToRoom(const FString& RoomID, const FString& UserID);
	// forget room
	void ForgetRoom(const FString& RoomID);

	void SendMessage(const FString& RoomID, const FString& Message, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());

	void GetPastMessages(const FString& RoomID, const FString& PrevBatchToken);



	void Initialize();

	~MatrixSession();

	MatrixSession();

	// To call all the invites previously read
	TArray<FMatrixInvitedRoom> GetFriendInvites();
	
	// To call all the invites previously read
	TArray<FMatrixInvitedRoom> GetTeamInvites();
	
	// only fetchs available friend rooms
	TMap<FString, TSharedPtr<MatrixRoomSession>> GetFriendRooms();

	TMap<FString, TSharedPtr<MatrixRoomSession>> GetTeamRooms() const;

	bool IsRoomDirectConnection(FString RoomID);
	bool IsUserDirectConnection(FString UserID) const;

	//// TODO: remove if this not used
	//FString GetAccessToken()
	//{
	//	return AccessToken;
	//}

	FString GetLoggedInUserID() const
	{
		return m_LoggedInUserID;
	}

	// room specific
	void GetRoomMembers(const FString& RoomID, EMatrixMembership Membership,
					const MatrixNativeAPI::FOnMatrixRoomMembersResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixRoomMembersResponseDelegate());

	TSharedPtr<MatrixRoomSession> GetRoomSession(const FString& RoomID);

	// Get user relation i.e. friend, blocked, friend request sent, received
	EMatrixUserRelation GetUserRelation(const FString& UserID);

	// get user's online status
	EMatrixPresence GetPresenceForUser(const FString& UserID);

	FString GetFriendUserIDForRoom(const FString& RoomID);


	// Blocked functionalities
	TArray<FString> GetBlockedList() const;
	void BlockUser(const FString& UserID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());
	void UnblockUser(const FString& UserID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());

	FString GetRoomIDForFriend(FString UserID) const;


	// Kick User
	void KickUserFromRoom(const FString& RoomID, const FString& UserID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());

	void UpdateRoomName(const FString& RoomID, const FString& NewName, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());

	//TODO: change name to for room
	void SendTypingNotification(const FString& RoomID, bool bIsTyping);
	void UpdateFullReadMarkerForRoom(const FString& RoomID, const FString& FullyReadEventID, const FString& ReadEventID = "");

	void SendChatEventNotification(const FString& InRoomID, bool bHasUnreadEvents, bool bIsFriendChatEvent);

	bool HasTeamChatNewNotifications() const;
	bool HasFriendChatNewNotifications() const;

	/*void SetTeamChatNotificationAsRead();
	void SetFriendChatNotificationAsRead();*/

	void ConfirmFriendRoom(const FString& RoomID);

	void CancelFriendRequest(const FString& UserID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());

	FMatrixInvitedRoom GetDirectInviteFromUser(const FString& UserID);

private:

	//userid, roomid map - all the friend connection including blocked friends 
	TMap<FString, FString> m_DirectConnections;

	// room id and notification status
	TSet<FString> m_TeamRoomsWithUnreadEvents;
	TSet<FString> m_FriendRoomsWithUnreadEvents;


	// userid - presense map
	TMap<FString, EMatrixPresence> m_Presence;

	// array of user_ids
	TArray<FString> m_BlockedUsers;

	// room id, sender
	TMap<FString, FString> m_DirectRoomInvitesProcessingBuffer;
	TSet<FString> m_DirectRoomLeaveProcessingBuffer;

	TMap<FString, FMatrixInvitedRoom> m_DirectRoomInvitesMap;
	TMap<FString, FMatrixInvitedRoom> m_TeamInvitesMap;

	// room info after they are fetched by UI
	// RoomID, RoomSession
	TMap<FString, TSharedPtr<MatrixRoomSession>> m_UnconfirmedFriendRooms; // waiting for the reuqest to be accepted
	TMap<FString, TSharedPtr<MatrixRoomSession>> m_TeamRoomsMap;
	TMap<FString, TSharedPtr<MatrixRoomSession>> m_FriendRoomsMap;

	// Used when to store the values when the update request is being sent while room is being created, left and joined
	// user id roomID
	TMap<FString, FString> m_DirectRoomUpdateBuffer;
	TMap<FString, FString> m_DirectRoomRemoveBuffer;

	// Direct connections exculding blocked users
	// UserID, RoomID
	TMap<FString, FString> m_AvailableDirectConnections;

	int m_TickDelay = 301;

	bool bIsStart = true;
	FString m_SinceTokenSync;

	Database* p_DB;

	FString m_AccessToken;
	FString m_LoggedInUserID;
	FString m_HomeServer;

	void ExportMembers() override;

	void ImportMembers(char** Vals) override;

	TSharedPtr<MatrixNativeAPI> m_NativeAPI;

	ELoginState m_LoginState = ELoginState::LoggedOut;
	ESyncState m_SyncState = ESyncState::Finished;

	bool bHasTeamChatNewNotifications = false;
	bool bHasFriendChatNewNotifications = false;
	//EMatrixCallState CallState = EMatrixCallState::Ended;

	//TPair<FString, FString> CurrentCallInfo;

	void SetupDatabase();

	/** Begin FTickableGameObject overrides */
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const;
	virtual bool IsTickableInEditor() const { return true; }
	/** End FTickableGameObject overrides */
	
	void OnLoginResponseReceivedCallback(EMatrixCoreError Status, MatrixLoginResponse Response, FString Error);
	void OnCreateRoomResponseCallback(EMatrixCoreError Status, FString RoomID, bool bIsDirect, FString DirectUserID, FString Error);
	void OnSyncResponseCallback(EMatrixCoreError Status, MatrixSyncResponse SyncResponse, bool bIsInitalSync, FString ErrorMsg);

	void OnJoinRoomResponseCallback(EMatrixCoreError Status, FString RoomID, FString Error);
	void OnLeaveRoomResponseCallback(EMatrixCoreError Status, FString RoomID, FString Error);

	void OnRoomMembersResponseCallback(EMatrixCoreError Status, MatrixRoomMembersResponse Response, FString Error);

	void OnGetMessagesResponseCallback(EMatrixCoreError Status, MatrixGetMessagesResponse Response, FString Error);

	// void UpdateCallState(EMatrixCallState State, FString RoomID, FString CallID);

	void SetLoginState(ELoginState State);

	// updates the direct connections - used in on sync response
	void UpdateDirectConnections(TMap<FString, FString> Connections);

	// updates the available connections and brodcast the event incase friend is added or removed
	void UpdateAvailableDirectConnections(TMap<FString, FString> Connections);

	//void OnEventSendSelfReadMarkerUpdate(FString RoomID, FString EventID);


	// for call processing and edge cases
	// call id 
	// TMap<FString, >
	// Broadcast - Callstate RoomID CallID
	// TODO: make it private and give access through only

	FOnCallEventReceived d_EventOnCallEventReceived;

	// call invite
	void SendCallInvite(const FString& RoomID, const FString& CallID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());
	void SendCallAnswer(const FString& RoomID, const FString& CallID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());
	void SendCallHangup(const FString& RoomID, const FString& CallID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());

	// calling related 
	friend void TesserconSession::SendMatrixCallInvite(const FString& RoomID);
	friend void TesserconSession::BindMatrixCallingEvents(bool bDoBind, MatrixSession& MSession);
	friend void TesserconSession::SendMatrixCallAnswer(const FString& RoomID, const FString& CallID);
	friend void TesserconSession::SendMatrixCallHangup(const FString& RoomID, const FString& CallID, const TesserconSession::FOnTesserconSessionResponseDelegate& TheDelegate);
	friend void TesserconSession::HangupCallInvite(const FString& RoomID, const FString& CallID, const TesserconSession::FOnTesserconSessionResponseDelegate& TheDelegate);

	bool HasDirectInviteFromUser(FString UserID);
	bool bIsProcessingRoomRequest = false;
};
