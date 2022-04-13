#include "MatrixSession.h"
#include "Algo/Reverse.h"

EXPORT_CLASS(MatrixSession)

MatrixSession::MatrixSession()
{
	UE_LOG(LogMatrixCore, Warning, TEXT(" In MatrixSession::MatrixSession() -------------------"));
}

void MatrixSession::ExportMembers()
{
	REGISTER_MEMBER(m_LoggedInUserID, ESQLiteDataType::TEXT, ESQLiteConstraint::UNIQUE);

	REGISTER_MEMBER(m_AccessToken, ESQLiteDataType::TEXT, ESQLiteConstraint::NONE);
	
	REGISTER_MEMBER(m_HomeServer, ESQLiteDataType::TEXT, ESQLiteConstraint::NONE);
}

void MatrixSession::ImportMembers(char** Vals)
{
	m_LoggedInUserID = FString(ANSI_TO_TCHAR(Vals[0]));

	m_AccessToken = FString(ANSI_TO_TCHAR(Vals[1]));

	m_HomeServer = FString(ANSI_TO_TCHAR(Vals[2]));
}

void MatrixSession::Initialize()
{
	m_NativeAPI = TSharedPtr<MatrixNativeAPI>(new MatrixNativeAPI());

	if (m_NativeAPI == nullptr) return;

	// intialize the seed for transaction ID's
	srand(time(NULL));

	p_DB = Database::GetDatabaseInstance();

	if (p_DB != nullptr)
	{
		//SetupDatabase();
	}

	// TODO: check if the postion of this line can be moved to the login function
	m_NativeAPI->d_OnLoginResponseReceived.BindRaw(this, &MatrixSession::OnLoginResponseReceivedCallback);
	m_NativeAPI->d_OnCreateRoomResponseReceived.BindRaw(this, &MatrixSession::OnCreateRoomResponseCallback);
	m_NativeAPI->d_OnSyncResponseReceived.BindRaw(this, &MatrixSession::OnSyncResponseCallback);
	m_NativeAPI->d_OnJoinRoomResponseReceived.BindRaw(this, &MatrixSession::OnJoinRoomResponseCallback);
	m_NativeAPI->d_OnLeaveRoomResponseReceived.BindRaw(this, &MatrixSession::OnLeaveRoomResponseCallback);
	m_NativeAPI->d_OnRoomMembersResponseReceived.BindRaw(this, &MatrixSession::OnRoomMembersResponseCallback);
	m_NativeAPI->d_OnGetMessagesResponseReceived.BindRaw(this, &MatrixSession::OnGetMessagesResponseCallback);
}


void MatrixSession::SetupDatabase()
{
	if (p_DB != nullptr)
	{
		p_DB->CreateTable<MatrixSession>();
	}


	MatrixSession* DBSession = p_DB->Read<MatrixSession>("");

	if (DBSession->m_AccessToken.IsEmpty() || DBSession->m_LoggedInUserID.IsEmpty() || DBSession->m_HomeServer.IsEmpty()) return;

	m_LoginState = ELoginState::LoggedIn;
	m_AccessToken = DBSession->m_AccessToken;
	m_LoggedInUserID = DBSession->m_LoggedInUserID;
	m_HomeServer = DBSession->m_HomeServer;
}

MatrixSession::~MatrixSession()
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->d_OnLoginResponseReceived.Unbind();
		m_NativeAPI->d_OnCreateRoomResponseReceived.Unbind();
		m_NativeAPI->d_OnSyncResponseReceived.Unbind();
		m_NativeAPI->d_OnJoinRoomResponseReceived.Unbind();
		m_NativeAPI->d_OnLeaveRoomResponseReceived.Unbind();
		m_NativeAPI->d_OnRoomMembersResponseReceived.Unbind();
		m_NativeAPI->d_OnGetMessagesResponseReceived.Unbind();
	}

	UE_LOG(LogMatrixCore, Warning, TEXT(" In MatrixSession::~MatrixSession() -------------------"));

}

void MatrixSession::Login(const FString& Username, const FString& Password)
{
	if (m_NativeAPI != nullptr)
	{
		m_AccessToken = "";
		SetLoginState(ELoginState::LogginIn);

		m_NativeAPI->Login(Username, Password);
	}
}

void MatrixSession::OnLoginResponseReceivedCallback(EMatrixCoreError Status, MatrixLoginResponse Response, FString Error)
{

	if (Status == EMatrixCoreError::Success)
	{
		UE_LOG(LogMatrixCore, Warning, TEXT("LoginDelegate Lamda : Access Token %s"), *Response.AccessToken);

		m_AccessToken = Response.AccessToken;
		SetLoginState(ELoginState::LoggedIn);
		m_LoggedInUserID = Response.UserID;
		m_HomeServer = Response.HomeServer;

		if (p_DB != nullptr)
		{
			p_DB->Write(this);
		}

		// TODO: remove for temp testing
		m_TickDelay = 0;
	}
	else
	{
		SetLoginState(ELoginState::LoggedOut);
		UE_LOG(LogMatrixCore, Warning, TEXT("LoginDelegate Lamda [Failed]: %s "), *Error);
	}

	//EventLoginResponse.Broadcast(Status, Error);
}

void MatrixSession::Logout()
{
	SetLoginState(ELoginState::LoggingOut);

	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->Logout(m_AccessToken);
	}


	ESQLiteResult Status = ESQLiteResult::EMPTY;

	if (p_DB != nullptr)
	{
		Status = p_DB->ClearTable<MatrixSession>();
	}
	if (Status == ESQLiteResult::OK)
	{
		SetLoginState(ELoginState::LoggedOut);
		m_AccessToken = "";
		m_LoggedInUserID = "";
		m_HomeServer = "";
	}	
}

void MatrixSession::SetLoginState(ELoginState State)
{
	m_LoginState = State;
	d_EventOnLoginStateChange.Broadcast(State);
}


void MatrixSession::Tick(float DeltaTime)
{
	//TickDelay++;
	if (/*TickDelay > 300 &&*/ m_LoginState == ELoginState::LoggedIn && m_SyncState == ESyncState::Finished && !bIsProcessingRoomRequest)
	{
		//UE_LOG(LogMatrixCore, Warning, TEXT("MatrixSession::Tick delta = %f --------------"), DeltaTime);
		if (m_NativeAPI != nullptr)
		{
			if (bIsStart)
			{
				m_NativeAPI->Sync(m_AccessToken, "", true);
			}
			else
			{
				m_NativeAPI->Sync(m_AccessToken, m_SinceTokenSync, false);
			}

			m_SyncState = ESyncState::Started;			
		}
		//TickDelay = 0;
	}
	/*else
	{
		TickDelay++;
	}*/

	//UE_LOG(LogMatrixCore, Warning, TEXT("MatrixSession::Tick delta = %f --------------"), DeltaTime);
}

void MatrixSession::OnSyncResponseCallback(EMatrixCoreError Status, MatrixSyncResponse SyncResponse, bool bIsInitalSync, FString ErrorMsg)
{
	if (Status == EMatrixCoreError::Success)
	{
		if (bIsInitalSync)
		{
			bIsStart = false;
		}
		m_SinceTokenSync = SyncResponse.NextBatch;

		bool bHasNewChanges = false;

		if (SyncResponse.AccountData.bHasBlockedUserUpdate)
		{
			bHasNewChanges = true;
			m_BlockedUsers = SyncResponse.AccountData.BlockedUsers;
			UpdateAvailableDirectConnections(m_DirectConnections);
		}

		if (SyncResponse.AccountData.bHasDirectConnectionsUpdate)
		{
			bHasNewChanges = true;
			UpdateDirectConnections(SyncResponse.AccountData.DirectConnections);
		}

		if (SyncResponse.Rooms.InvitedRooms.Num() > 0)
		{
			bHasNewChanges = true;
			for (auto RoomInvite : SyncResponse.Rooms.InvitedRooms)
			{
				if (RoomInvite.bIsDirect)
				{
					m_DirectRoomInvitesMap.Add(RoomInvite.RoomID, RoomInvite);
					d_EventOnFriendRequestReceived.Broadcast(RoomInvite);
				}
				else
				{
					m_TeamInvitesMap.Add(RoomInvite.RoomID, RoomInvite);
					d_EventOnTeamRequestReceived.Broadcast(RoomInvite);
				}
			}
		}

		if (SyncResponse.Rooms.LeftRooms.Num() > 0)
		{
			bHasNewChanges = true;

			// remove from the joined room list maintained in the session
			for (auto Room : SyncResponse.Rooms.LeftRooms)
			{
				// TODO: move to another function to resuse code
				if (IsRoomDirectConnection(Room)/* add more cases here*/)
				{
					if (m_FriendRoomsMap.Contains(Room))
					{
						TPair<FString, TSharedPtr<MatrixRoomSession>> RoomSessionPair(Room, *m_FriendRoomsMap.Find(Room));
						
						// check the edge case where the session has to removed
						m_FriendRoomsMap.Remove(Room);
					}

					else if (m_UnconfirmedFriendRooms.Contains(Room))
					{
						// TODO: what is the use?
						// TPair<FString, TSharedPtr<MatrixRoomSession>> RoomSessionPair(Room, *FriendRoomsMap.Find(Room));
						m_UnconfirmedFriendRooms.Remove(Room);
					}
				}
				else if (m_TeamRoomsMap.Contains(Room))
				{
					TPair<FString, TSharedPtr<MatrixRoomSession>> RoomSessionPair(Room, *m_TeamRoomsMap.Find(Room));

					d_EventOnTeamRemoved.Broadcast(Room);
					// remove the room session pair
					m_TeamRoomsMap.Remove(Room);
				}
				else if (m_DirectRoomInvitesMap.Contains(Room))
				{
					// when friend request is removed
					m_DirectRoomInvitesMap.Remove(Room);
					d_EventOnReceivedFriendRequestWithdrawn.Broadcast(Room);
				}
				else if (m_TeamInvitesMap.Contains(Room))
				{
					// when team request is removed
					m_TeamInvitesMap.Remove(Room);
					d_EventOnReceivedTeamRequestWithdrawn.Broadcast(Room);
				}
						
				
			}
		}

		if (SyncResponse.Rooms.JoinedRooms.Num() > 0)
		{
			bHasNewChanges = true;
			bool bShouldCallDirectConnectionsUpdate = false;

			for (auto Room : SyncResponse.Rooms.JoinedRooms)
			{
				// TODO: Move to a function ----- reuse of code
				if (IsRoomDirectConnection(Room.RoomID))
				{
					if (m_FriendRoomsMap.Contains(Room.RoomID))
					{
						TSharedPtr<MatrixRoomSession> RoomSession = *m_FriendRoomsMap.Find(Room.RoomID);
						if (RoomSession != nullptr)
						{
							RoomSession->AddToSyncBuffer(Room);
						}
					}
					else
					{
						if (!m_UnconfirmedFriendRooms.Contains(Room.RoomID))
						{
							TSharedPtr<MatrixRoomSession> RoomSession = TSharedPtr<MatrixRoomSession>(new MatrixRoomSession(Room.RoomID, IsRoomDirectConnection(Room.RoomID)));
							TPair<FString, TSharedPtr<MatrixRoomSession>> Pair(Room.RoomID, RoomSession);
							m_UnconfirmedFriendRooms.Add(Pair);
							RoomSession->AddToSyncBuffer(Room);
						}
						else
						{
							TSharedPtr<MatrixRoomSession> RoomSession = *m_UnconfirmedFriendRooms.Find(Room.RoomID);
							if (RoomSession != nullptr)
							{
								RoomSession->AddToSyncBuffer(Room);
							}
						}
					}
				}
				else
				{
					if (!m_TeamRoomsMap.Contains(Room.RoomID))
					{
						TSharedPtr<MatrixRoomSession> RoomSession = TSharedPtr<MatrixRoomSession>(new MatrixRoomSession(Room.RoomID, IsRoomDirectConnection(Room.RoomID)));
						TPair<FString, TSharedPtr<MatrixRoomSession>> Pair(Room.RoomID, RoomSession);
						m_TeamRoomsMap.Add(Pair);
						d_EventOnTeamAdded.Broadcast(Room.RoomID);
						RoomSession->AddToSyncBuffer(Room);
					}
					else
					{
						TSharedPtr<MatrixRoomSession> RoomSession = *m_TeamRoomsMap.Find(Room.RoomID);
						if (RoomSession != nullptr)
						{
							RoomSession->AddToSyncBuffer(Room);
						}

					}
				}

				// -----------------------------------------------

				// TODO: update the call invites to check 

				if (Room.RoomEvents.CallInvites.Num() > 0 && !bIsInitalSync)
				{
					FDateTime TimeNow = FDateTime::UtcNow();
									
					//TArray<FMatrixCallInvite> ValidCallInvites;
					for (auto CallInvite : Room.RoomEvents.CallInvites)
					{
						if (TimeNow < CallInvite.ExpireTime)
						{
							d_EventOnCallEventReceived.Broadcast(EMatrixCallEventType::Invite, Room.RoomID, CallInvite);
						}
					}
				}

				if (Room.RoomEvents.CallAnswers.Num() > 0 && !bIsInitalSync)
				{
					for (auto CallAnswer : Room.RoomEvents.CallAnswers)
					{
						//call answer event
						d_EventOnCallEventReceived.Broadcast(EMatrixCallEventType::Answer, Room.RoomID, CallAnswer);
					}
				}

				if (Room.RoomEvents.CallHangUps.Num() > 0 && !bIsInitalSync)
				{
					for (auto CallHangup : Room.RoomEvents.CallHangUps)
					{
						// call hangup event
						d_EventOnCallEventReceived.Broadcast(EMatrixCallEventType::Hangup, Room.RoomID, CallHangup);
					}
				}
			}
			
		}

		if (SyncResponse.PresenceMap.Num() > 0)
		{
			bHasNewChanges = true;
			for (auto Pair : SyncResponse.PresenceMap)			{
				m_Presence.Add(Pair);
			}
		}

		if (bHasNewChanges)
		{
			// check if sync response had any data and call the event if it has any!!!
			d_EventSyncResponse.Broadcast();
		}
		
	}

	m_SyncState = ESyncState::Finished;
}

TStatId MatrixSession::GetStatId() const
{
	return TStatId();
}

// create room
void MatrixSession::CreateRoom(const TArray<FString>& UserIds, bool bIsDirect, const FString& RoomName, 
	const MatrixNativeAPI::FOnMatrixCreateRoomReponseDelegate& TheDelegate)
{
	if (UserIds.Num() < 1)
	{
		// send a response saying that atleast one user must be in a team
		TheDelegate.ExecuteIfBound(EMatrixCoreError::FORBIDDEN, "");
		return;
	}
	else
	{
		if (bIsDirect && IsUserDirectConnection(UserIds[0]))
		{
			// warn if a connection already exists
			TheDelegate.ExecuteIfBound(EMatrixCoreError::FORBIDDEN, "");
			return;
		}
		

		if (m_NativeAPI != nullptr)
		{ 
			if (UserIds.Num() <= MATRIX_TEAM_MAX_USERS)
			{
				m_NativeAPI->CreateRoom(m_AccessToken, UserIds, bIsDirect, RoomName, TheDelegate);
				bIsProcessingRoomRequest = true;
			}
			else
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNAUTHORIZED, "");
			}
		}
	}
}

void MatrixSession::OnCreateRoomResponseCallback(EMatrixCoreError Status, FString RoomID, bool bIsDirect, FString DirectUserID, FString Error)
{
	//EventCreateRoomResponse.Broadcast(Status, Error);
	if (Status == EMatrixCoreError::Success)
	{
		if (bIsDirect && m_NativeAPI != nullptr)
		{
			// call update the account api update
			TMap<FString, FString>  NewConnections = m_DirectConnections;
			NewConnections.Add(DirectUserID, RoomID);

			// Create a room session
			TSharedPtr<MatrixRoomSession> RoomSession = TSharedPtr<MatrixRoomSession>(new MatrixRoomSession(RoomID, true));
			TPair<FString, TSharedPtr<MatrixRoomSession>> Pair(RoomID, RoomSession);

			// TODO: add this to unconfirmed friend requests ---- check if this valid??
			m_UnconfirmedFriendRooms.Add(Pair);
			//FriendRoomsMap.Add(Pair);

			m_DirectRoomUpdateBuffer.Add(DirectUserID, RoomID);

			MatrixNativeAPI::FOnMatrixAPIResponseDelegate OnResponseCallback;
			OnResponseCallback.BindLambda([this, DirectUserID, NewConnections](EMatrixCoreError Status)
			{
				if (Status == EMatrixCoreError::Success)
				{
					UpdateDirectConnections(NewConnections);
					m_DirectRoomUpdateBuffer.Remove(DirectUserID);
				}

				bIsProcessingRoomRequest = false;
			});

			m_NativeAPI->UpdateDirectConnections(m_AccessToken, NewConnections, m_LoggedInUserID, OnResponseCallback);

		}
		else if (!bIsDirect)
		{
			// TODO: check if this is needed? create a new function for this 
			TSharedPtr<MatrixRoomSession> RoomSession = TSharedPtr<MatrixRoomSession>(new MatrixRoomSession(RoomID, false));
			TPair<FString, TSharedPtr<MatrixRoomSession>> Pair(RoomID, RoomSession);
			m_TeamRoomsMap.Add(Pair);
			d_EventOnTeamAdded.Broadcast(RoomID);

			//bHasNewTeams = true;
			bIsProcessingRoomRequest = false;
		}
	}
	else
	{
		bIsProcessingRoomRequest = false;
	}
}

// join room
void MatrixSession::JoinRoom(const FString& RoomID, bool bIsDirect, const FString& Sender, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	// TODO: change the input type to a struct if needed
	if (m_NativeAPI != nullptr)
	{
		if (bIsDirect)
		{
			m_DirectRoomInvitesProcessingBuffer.Add(RoomID, Sender);
		}

		m_NativeAPI->JoinRoom(m_AccessToken, RoomID, TheDelegate);

		bIsProcessingRoomRequest = true;
	}
}

void MatrixSession::OnJoinRoomResponseCallback(EMatrixCoreError Status, FString RoomID, FString Error)
{
	if (Status == EMatrixCoreError::Success)
	{
		if (m_DirectRoomInvitesProcessingBuffer.Contains(RoomID))
		{
			FString UserID;
			m_DirectRoomInvitesProcessingBuffer.RemoveAndCopyValue(RoomID, UserID);

			TMap<FString, FString>  NewConnections = m_DirectConnections;
			NewConnections.Add(UserID, RoomID);

			m_DirectRoomUpdateBuffer.Add(UserID, RoomID);

			MatrixNativeAPI::FOnMatrixAPIResponseDelegate OnResponseCallback;
			OnResponseCallback.BindLambda([this, UserID, NewConnections](EMatrixCoreError Status)
			{
				if (Status == EMatrixCoreError::Success)
				{
					UpdateDirectConnections(NewConnections);
					m_DirectRoomUpdateBuffer.Remove(UserID);
				}

				bIsProcessingRoomRequest = false;
			});

			if (m_NativeAPI != nullptr)
			{
				m_NativeAPI->UpdateDirectConnections(m_AccessToken, NewConnections, m_LoggedInUserID, OnResponseCallback);
			}

			TSharedPtr<MatrixRoomSession> RoomSession = TSharedPtr<MatrixRoomSession>(new MatrixRoomSession(RoomID, true));
			TPair<FString, TSharedPtr<MatrixRoomSession>> Pair(RoomID, RoomSession);
			m_FriendRoomsMap.Add(Pair);
		}
		else
		{
			TSharedPtr<MatrixRoomSession> RoomSession = TSharedPtr<MatrixRoomSession>(new MatrixRoomSession(RoomID, false));
			TPair<FString, TSharedPtr<MatrixRoomSession>> Pair(RoomID, RoomSession);

			m_TeamRoomsMap.Add(Pair);
			d_EventOnTeamAdded.Broadcast(RoomID);


			//bHasNewTeams = true;
			bIsProcessingRoomRequest = false;

		}

		if (m_DirectRoomInvitesMap.Contains(RoomID))
		{
			m_DirectRoomInvitesMap.Remove(RoomID);
		}

		if (m_TeamInvitesMap.Contains(RoomID))
		{
			m_TeamInvitesMap.Remove(RoomID);
		}
	}
	else
	{
		bIsProcessingRoomRequest = false;
	}
}

TArray<FMatrixInvitedRoom> MatrixSession::GetFriendInvites()
{
	TArray<FMatrixInvitedRoom> FriendInvitesResult;
	m_DirectRoomInvitesMap.GenerateValueArray(FriendInvitesResult);

	Algo::Reverse(FriendInvitesResult);

	return FriendInvitesResult;
}


TArray<FMatrixInvitedRoom> MatrixSession::GetTeamInvites()
{
	TArray<FMatrixInvitedRoom> TeamInvitesResult;
	m_TeamInvitesMap.GenerateValueArray(TeamInvitesResult);

	Algo::Reverse(TeamInvitesResult);

	return TeamInvitesResult;
}


// leave room
void MatrixSession::LeaveRoom(const FString& RoomID, bool bIsDirect, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
	{
		if (bIsDirect)
		{
			m_DirectRoomLeaveProcessingBuffer.Add(RoomID);
		}

		m_NativeAPI->LeaveRoom(m_AccessToken, RoomID, TheDelegate);

		bIsProcessingRoomRequest = true;
	}
}

void MatrixSession::OnLeaveRoomResponseCallback(EMatrixCoreError Status, FString RoomID, FString Error)
{
	if (Status == EMatrixCoreError::Success)
	{
		if (m_DirectRoomLeaveProcessingBuffer.Contains(RoomID) && m_NativeAPI != nullptr)
		{
			m_DirectRoomLeaveProcessingBuffer.Remove(RoomID);

			TMap<FString, FString>  NewConnections = m_DirectConnections;
			FString FoundUserID;
			for (auto Pair : NewConnections)
			{
				if (Pair.Value == RoomID)
				{
					FoundUserID = Pair.Key;
					break;
				}
			}
			FString RoomIDRemoved;
			NewConnections.RemoveAndCopyValue(FoundUserID, RoomIDRemoved);

			m_DirectRoomRemoveBuffer.Add(FoundUserID, RoomID);

			MatrixNativeAPI::FOnMatrixAPIResponseDelegate OnResponseCallback;
			OnResponseCallback.BindLambda([this, FoundUserID, NewConnections](EMatrixCoreError Status)
			{
				if (Status == EMatrixCoreError::Success)
				{
					UpdateDirectConnections(NewConnections);
					m_DirectRoomRemoveBuffer.Remove(FoundUserID);
				}

				bIsProcessingRoomRequest = false;

			});


			m_NativeAPI->UpdateDirectConnections(m_AccessToken, NewConnections, m_LoggedInUserID, OnResponseCallback);
		}
		else
		{
			bIsProcessingRoomRequest = false;
		}

		if (m_DirectRoomInvitesMap.Contains(RoomID))
		{
			m_DirectRoomInvitesMap.Remove(RoomID);
		}

		if (m_TeamInvitesMap.Contains(RoomID))
		{
			m_TeamInvitesMap.Remove(RoomID);
		}
	}
	else
	{
		bIsProcessingRoomRequest = false;
	}
}

// forget room
void MatrixSession::ForgetRoom(const FString& RoomID)
{
	if (m_NativeAPI != nullptr)
	{

	}
}

void MatrixSession::GetRoomMembers(const FString& RoomID, EMatrixMembership Membership, const MatrixNativeAPI::FOnMatrixRoomMembersResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->GetRoomMembers(m_AccessToken, RoomID, Membership, TheDelegate);
	}
}

void MatrixSession::OnRoomMembersResponseCallback(EMatrixCoreError Status, MatrixRoomMembersResponse Response, FString Error)
{
	if (EMatrixCoreError::Success == Status)
	{
		// TODO: add that particular room function
		TSharedPtr<MatrixRoomSession> RoomSession = GetRoomSession(Response.RoomID);
		if (RoomSession != nullptr)
		{
			RoomSession->OnReceivedMembersCallback(Response.UserIDs, Response.Membership);
		}
	}
}

TSharedPtr<MatrixRoomSession> MatrixSession::GetRoomSession(const FString& RoomID)
{
	TSharedPtr<MatrixRoomSession> Result;
	if (m_FriendRoomsMap.Contains(RoomID))
	{
		Result = *m_FriendRoomsMap.Find(RoomID);
	}
	else if (m_TeamRoomsMap.Contains(RoomID))
	{
		Result = *m_TeamRoomsMap.Find(RoomID);
	}
	return Result;
}

void MatrixSession::InviteUserToRoom(const FString& RoomID, const FString& UserID)
{
	if (m_NativeAPI != nullptr)
		m_NativeAPI->InviteToRoom(m_AccessToken, UserID, RoomID);
}

void MatrixSession::SendMessage(const FString& RoomID, const FString& Message, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
		m_NativeAPI->SendMessage(m_AccessToken, RoomID, Message, TheDelegate);
}


void MatrixSession::SendCallInvite(const FString& RoomID, const FString& CallID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	// native api and if already a call is not happening
	if (m_NativeAPI != nullptr)
	{		
		m_NativeAPI->SendCallInvite(m_AccessToken, RoomID, CallID, TheDelegate);
		return;
	}
	TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
}


void MatrixSession::SendCallAnswer(const FString& RoomID, const FString& CallID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->SendCallAnswer(m_AccessToken, RoomID, CallID, TheDelegate);
		return;
	}
	TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
}

void MatrixSession::SendCallHangup(const FString& RoomID, const FString& CallID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->SendCallHangup(m_AccessToken, RoomID, CallID, EMatrixCallHangUpReason::None, TheDelegate);
		return;
	}
	TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
}

void MatrixSession::GetPastMessages(const FString& RoomID, const FString& PrevBatchToken)
{
	if (m_NativeAPI != nullptr)
		m_NativeAPI->GetMessages(m_AccessToken, RoomID, PrevBatchToken);
}

void MatrixSession::OnGetMessagesResponseCallback(EMatrixCoreError Status, MatrixGetMessagesResponse Response, FString Error)
{
	if (Status == EMatrixCoreError::Success)
	{
		TSharedPtr<MatrixRoomSession> RoomSession = GetRoomSession(Response.RoomID);

		if (RoomSession != nullptr)
			RoomSession->OnGetMessagesResponse(Response);
	}	
}

EMatrixUserRelation MatrixSession::GetUserRelation(const FString& UserID)
{
	// TODO: add if friend request is received
	if (m_BlockedUsers.Contains(UserID))
		return EMatrixUserRelation::Blocked;
	
	if (HasDirectInviteFromUser(UserID))
		return EMatrixUserRelation::RequestReceived;
	
	if (IsUserDirectConnection(UserID) && m_UnconfirmedFriendRooms.Contains(GetRoomIDForFriend(UserID)) )
		return EMatrixUserRelation::RequestSent;
	
	if (IsUserDirectConnection(UserID))
		return EMatrixUserRelation::Friend;
	
	return EMatrixUserRelation::NotFriend;
}


EMatrixPresence MatrixSession::GetPresenceForUser(const FString& UserID)
{
	if (m_Presence.Contains(UserID))
		return *m_Presence.Find(UserID);
	
	return EMatrixPresence::Unavailable;
}

TMap<FString, TSharedPtr<MatrixRoomSession>> MatrixSession::GetFriendRooms()
{
	TMap<FString, TSharedPtr<MatrixRoomSession>> AvailableFriendRooms;

	for (auto UserRoomPair : m_AvailableDirectConnections)
		// key = userid, value = room id
		if (m_FriendRoomsMap.Contains(UserRoomPair.Value))
			AvailableFriendRooms.Add(UserRoomPair.Value, *m_FriendRoomsMap.Find(UserRoomPair.Value));

	return AvailableFriendRooms;
}

TMap<FString, TSharedPtr<MatrixRoomSession>> MatrixSession::GetTeamRooms() const
{
	return m_TeamRoomsMap;
}

bool MatrixSession::IsUserDirectConnection(FString UserID) const
{
	return (m_DirectConnections.Contains(UserID) || m_DirectRoomUpdateBuffer.Contains(UserID)) &&
		!m_DirectRoomRemoveBuffer.Contains(UserID); 
}

bool MatrixSession::IsRoomDirectConnection(FString RoomID)
{
	TArray<FString> DirectConnectionsRoomIDs;
	m_DirectConnections.GenerateValueArray(DirectConnectionsRoomIDs);

	TArray<FString> NewDirectConnectionsRoomIDs;
	m_DirectRoomUpdateBuffer.GenerateValueArray(NewDirectConnectionsRoomIDs);

	TArray<FString> RemovedDirectConnectionsRoomIDs;
	m_DirectRoomRemoveBuffer.GenerateValueArray(RemovedDirectConnectionsRoomIDs);

	return (DirectConnectionsRoomIDs.Contains(RoomID) || NewDirectConnectionsRoomIDs.Contains(RoomID)) &&
		!m_DirectRoomRemoveBuffer.Contains(RoomID);
}

FString MatrixSession::GetFriendUserIDForRoom(const FString& RoomID)
{
	if (IsRoomDirectConnection(RoomID))
	{
		for (auto Pair : m_DirectConnections)
		{
			if (Pair.Value == RoomID)
			{
				return Pair.Key;
			}
		}
		for (auto Pair : m_DirectRoomUpdateBuffer)
		{
			if (Pair.Value == RoomID)
			{
				return Pair.Key;
			}
		}
	}
	return "";
}

TArray<FString> MatrixSession::GetBlockedList() const
{
	return m_BlockedUsers;
}


void MatrixSession::BlockUser(const FString& UserID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	TArray<FString> NewBlockedList = m_BlockedUsers;
	NewBlockedList.Add(UserID);

	if (m_NativeAPI != nullptr)
	{
		MatrixNativeAPI::FOnMatrixAPIResponseDelegate InnerDelegate;
		InnerDelegate.BindLambda([this, NewBlockedList, TheDelegate](EMatrixCoreError Status)
		{
			if (EMatrixCoreError::Success == Status)
			{
				m_BlockedUsers = NewBlockedList;
			}
			
			TheDelegate.ExecuteIfBound(Status);
		});
		m_NativeAPI->UpdateIgnoreList(m_AccessToken, NewBlockedList, m_LoggedInUserID, InnerDelegate);
	}
	else
	{
		TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
	}
}


void MatrixSession::UnblockUser(const FString& UserID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	if (m_BlockedUsers.Contains(UserID))
	{
		TArray<FString> NewBlockedList = m_BlockedUsers;
		NewBlockedList.Remove(UserID); 

		if (m_NativeAPI != nullptr)
		{
			MatrixNativeAPI::FOnMatrixAPIResponseDelegate InnerDelegate;
			InnerDelegate.BindLambda([this, NewBlockedList, TheDelegate](EMatrixCoreError Status)
			{
				if (EMatrixCoreError::Success == Status)
				{
					m_BlockedUsers = NewBlockedList;
				}

				TheDelegate.ExecuteIfBound(Status);
			});
			m_NativeAPI->UpdateIgnoreList(m_AccessToken, NewBlockedList, m_LoggedInUserID, InnerDelegate);
		}
	}
	else
	{
		TheDelegate.ExecuteIfBound(EMatrixCoreError::NOT_FOUND);
	}	
}

FString MatrixSession::GetRoomIDForFriend(FString UserID) const
{
	if (m_DirectConnections.Contains(UserID))
		return *m_DirectConnections.Find(UserID);
	
	if (m_DirectRoomUpdateBuffer.Contains(UserID))
		return *m_DirectRoomUpdateBuffer.Find(UserID);

	return "";
}

void MatrixSession::UpdateDirectConnections(TMap<FString, FString> Connections)
{
	// update available direct connections before sending the events
	UpdateAvailableDirectConnections(Connections);

	m_DirectConnections = Connections;
}

void MatrixSession::UpdateAvailableDirectConnections(TMap<FString, FString> Connections)
{
	TMap<FString, FString> NewAvailableConnections;

	for (auto UserRoomPair : Connections)
		// not in block list or waiting for the friend request to be accepted
		if (!m_BlockedUsers.Contains(UserRoomPair.Key) && !m_UnconfirmedFriendRooms.Contains(UserRoomPair.Value))
			NewAvailableConnections.Add(UserRoomPair);
	
	TSet<FString> OldList;
	TSet<FString> NewList;

	TArray<FString> OldFriendsArray;
	TArray<FString> NewFriendsArray;
	m_AvailableDirectConnections.GenerateKeyArray(OldFriendsArray);
	NewAvailableConnections.GenerateKeyArray(NewFriendsArray);

	OldList.Append(OldFriendsArray);
	NewList.Append(NewFriendsArray);

	TArray<FString> NewConnections;
	TArray<FString> LeftConnections;

//	GetDifferenceBetweenSets(NewList, OldList, NewConnections, LeftConnections);

	m_AvailableDirectConnections = NewAvailableConnections;

	// events for change in direct connections
	for (FString& User : NewConnections)
		d_EventOnFriendAdded.Broadcast(User);

	for (FString& User : LeftConnections)
		d_EventOnFriendRemoved.Broadcast(User);
}

void MatrixSession::KickUserFromRoom(const FString& RoomID, const FString& UserID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
		m_NativeAPI->KickUserFromRoom(m_AccessToken, RoomID, UserID, "", TheDelegate);
	
	else
		TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
}

void MatrixSession::UpdateRoomName(const FString& RoomID, const FString& NewName, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
		m_NativeAPI->UpdateRoomName(m_AccessToken, RoomID, NewName, TheDelegate);
	
	else
		TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
}


void MatrixSession::SendTypingNotification(const FString& RoomID, bool bIsTyping)
{
	if (m_NativeAPI != nullptr)
		m_NativeAPI->SendTypingNotificationForRoom(m_AccessToken, RoomID, m_LoggedInUserID, bIsTyping, 3000);
}

void MatrixSession::UpdateFullReadMarkerForRoom(const FString& RoomID, const FString& FullyReadEventID, const FString& ReadEventID)
{
	if (m_NativeAPI != nullptr)
		m_NativeAPI->UpdateFullyReadMarkerForRoom(m_AccessToken, RoomID, FullyReadEventID, ReadEventID);
}

//void MatrixSession::OnEventSendSelfReadMarkerUpdate(FString RoomID, FString EventID)
//{
//	UpdateFullReadMarkerForRoom(RoomID, EventID, EventID);
//}

void MatrixSession::SendChatEventNotification(const FString& InRoomID, bool bHasUnreadEvents, bool bIsFriendChatEvent)
{
	
	if (bIsFriendChatEvent)
	{
		if (m_FriendRoomsWithUnreadEvents.Contains(InRoomID))
		{
			if (!bHasUnreadEvents)
				m_FriendRoomsWithUnreadEvents.Remove(InRoomID);
		}
		else
		{
			if (bHasUnreadEvents)
				m_FriendRoomsWithUnreadEvents.Add(InRoomID);
		}

		bHasFriendChatNewNotifications = m_FriendRoomsWithUnreadEvents.Num() > 0 ;
		d_EventOnFriendChatEventNotification.Broadcast(bHasFriendChatNewNotifications);

	}
	else
	{
		// TODO: exclude the world chat - store the world chat room ID from tessercon cleint  and avoid it 
		if (m_TeamRoomsWithUnreadEvents.Contains(InRoomID))
		{
			if (!bHasUnreadEvents)
				m_TeamRoomsWithUnreadEvents.Remove(InRoomID);
		}
		else
		{
			if (bHasUnreadEvents)
				m_TeamRoomsWithUnreadEvents.Add(InRoomID);
		}

		bHasTeamChatNewNotifications = m_TeamRoomsWithUnreadEvents.Num() > 0;
		d_EventOnTeamChatEventNotification.Broadcast(bHasTeamChatNewNotifications);
	}
}

bool MatrixSession::HasTeamChatNewNotifications() const
{
	return bHasTeamChatNewNotifications;
}
bool MatrixSession::HasFriendChatNewNotifications() const
{
	return bHasFriendChatNewNotifications;
}

//void MatrixSession::SetTeamChatNotificationAsRead()
//{
//	bHasTeamChatNewNotifications = false;
//}
//void MatrixSession::SetFriendChatNotificationAsRead()
//{
//	bHasFriendChatNewNotifications = false;
//}

void MatrixSession::ConfirmFriendRoom(const FString& RoomID)
{
	if (!m_UnconfirmedFriendRooms.Contains(RoomID)) return;

	TSharedPtr<MatrixRoomSession> RoomSession = *m_UnconfirmedFriendRooms.Find(RoomID);

	m_FriendRoomsMap.Add(RoomID, RoomSession);

	m_UnconfirmedFriendRooms.Remove(RoomID);

	UpdateAvailableDirectConnections(m_DirectConnections);
}


bool MatrixSession::HasDirectInviteFromUser(FString UserID)
{
	for (auto InvitePair : m_DirectRoomInvitesMap)
		if (InvitePair.Value.Sender == UserID)
			return true;

	return false;
}

FMatrixInvitedRoom MatrixSession::GetDirectInviteFromUser(const FString& UserID)
{
	for (auto InvitePair : m_DirectRoomInvitesMap)
		if (InvitePair.Value.Sender == UserID)
			return InvitePair.Value;

	return FMatrixInvitedRoom();
}

void MatrixSession::CancelFriendRequest(const FString& UserID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString RoomID = GetRoomIDForFriend(UserID);

	if (m_NativeAPI != nullptr && RoomID != "")
		m_NativeAPI->UpdateMembershipStateManuallyToLeave(m_AccessToken, RoomID, UserID, TheDelegate);
	
	else
		TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
}