#pragma once

#include "CoreMinimal.h"
#include "MatrixCommon.h"
#include "MatrixNativeAPI.h"
#include "MatrixResponses.h"

class MATRIXCORE_API MatrixRoomSession
{

public:

	DECLARE_EVENT_OneParam(MatrixRoomSession, FOnGetPastMessagesReceived, FMatrixMesssageEvents)
	FOnGetPastMessagesReceived d_EventOnPastMessagesReceived;

	DECLARE_EVENT_OneParam(MatrixRoomSession, FOnSyncMessagesReceived, FMatrixMesssageEvents)
	FOnSyncMessagesReceived d_EventOnNewMessagesReceived;

	DECLARE_EVENT_OneParam(MatrixRoomSession, FOnRoomNameChanged, FString);
	FOnRoomNameChanged d_EventOnRoomNameChanged;

	DECLARE_EVENT_TwoParams(MatrixRoomSession, FOnRoomMemberMembershipChanged, FString, EMatrixMembership);
	FOnRoomMemberMembershipChanged d_EventOnMembershipChangedForMember;

	DECLARE_EVENT_OneParam(MatrixRoomSession, FOnRoomUsersTyping, TArray<FString>);
	FOnRoomUsersTyping d_EventOnUsersTyping;

	DECLARE_EVENT_OneParam(MatrixRoomSession, FOnNewUnreadEventsReceived, bool);
	FOnNewUnreadEventsReceived d_EventOnNewUnreadEvents;

	// TODO: change the name
	DECLARE_EVENT_OneParam(MatrixRoomSession, FOnSelfMessageEventReceived, FString);
	FOnSelfMessageEventReceived d_EventOnSelfMessageReceived;

	DECLARE_EVENT(MatrixRoomSession, FOnAdminEventsReceived);
	FOnAdminEventsReceived d_EventOnAdminsReceived;

	MatrixRoomSession(const FString& InRoomID, bool _bIsDirect);
	~MatrixRoomSession();

	
	// TODO: no need to call this , is it needed anymore ? including all the calling api's
	// void SendCallInvite(FString CallID);

	// TODO: do someting about this callbacks - maybe make matrixsession as friend class so that it can access the private members of this class
	void OnReceivedMembersCallback(const TArray<FString>& InUserIDs, EMatrixMembership InMembership);
	
	// TODO: remove
	// void OnCallInviteCallback(bool bWasSuccessful, FString _CallID);
	// 
	
	// TODO: even this has to be private
	void OnGetMessagesResponse(const MatrixGetMessagesResponse& Response);
	void AddToSyncBuffer(const FMatrixJoinedRoomSync& SyncObject);

	// void SendCallAnswer(FString CallID);

	// void SendCallHangup(FString CallID);

	void InviteUser(const FString& UserID);

	void SendMessage(const FString& Message, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());

	// update this 
	EMatrixGetMessagesStatus GetPastMessages();

	TArray<FMatrixMesssageEvents> GetMessageEvents() const;

	FString GetRoomID() const
	{
		return m_RoomID;
	}

	// Note: do not use this for the friends related name
	FString GetRoomName() const
	{
		return m_RoomName;
	}

	// TODO: is this needed anymore?
	TArray<FString> GetAdmins() const;

	bool IsAdminOfThisRoom() const;

	void KickUser(const FString& UserID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());

	void UpdateRoomName(const FString& NewName, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate = MatrixNativeAPI::FOnMatrixAPIResponseDelegate());

	TArray<FString> GetMemberListForMembership(EMatrixMembership Membership);

	int32 GetAvailableInviteCount();

	// call this function sending the typing status of the current user. bind this with editable textbox text changed
	void SendTypingNotification(bool bIsTyping) const;

	void UpdateFullyReadMarker();

	bool HasNewUnreadNotifications() const
	{
		return bHasUnreadNotifications;
	}


private:

	TArray<FMatrixMesssageEvents> m_MessageEvents;

	FString m_RoomID;
	FString m_RoomName;

	FString m_StartingBatchToken;
	FString m_PreviousBatchToken;
	bool bIsDirect; 

	class MatrixClient* p_Client;

	TArray<FString> m_Admins;

	TMap<FString, EMatrixMembership> m_MemberStatus;

	FString m_FullyReadMarker_Local;
	FString m_FullyReadMarker;
	bool bHasUnreadNotifications = false;
	bool bIsInitialSync = true;

	// TODO: Setup Banned Members later
	void ProcessMemberStatusChanges(const TMap<FString, EMatrixMembership>& NewMemberStatus);

	void GetMembers(EMatrixMembership Membership) const;

	bool bAreTokensInitialized = false;
	bool bHasFetchedAllPastMessages = false;
	bool bIsPastMessagesRequestProcessing = false;

	bool bIsConfirmedRoomForDirectConnection = false;

	FString GetEventIDOfLatestEvent();

	void SetHasNewUnreadNotification(bool bInHasNewNotifications);

	void CheckDirectConnectionValidity();

	void UpdateFullyReadMarker(const FString& InEventID);

};