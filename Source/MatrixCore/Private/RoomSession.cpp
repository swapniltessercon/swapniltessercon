#include "RoomSession.h"
#include "MatrixCore.h"

MatrixRoomSession::MatrixRoomSession(const FString& InRoomID, bool _bIsDirect)
	:m_RoomID(InRoomID), bIsDirect(_bIsDirect)
{
	if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
	{
		p_Client = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
	}

	GetMembers(EMatrixMembership::Invite);
	GetMembers(EMatrixMembership::Leave);
	GetMembers(EMatrixMembership::Join);
}


MatrixRoomSession::~MatrixRoomSession()
{

}

void MatrixRoomSession::GetMembers(EMatrixMembership Membership) const
{
	if (p_Client != nullptr)
		p_Client->GetSession().GetRoomMembers(m_RoomID, Membership);
}

void MatrixRoomSession::OnReceivedMembersCallback(const TArray<FString>& InUserIDs, EMatrixMembership InMembership)
{
	// send event to the UI
	TMap<FString, EMatrixMembership> ReceivedMembersMembership;
	for (auto UserID : InUserIDs)
	{
		ReceivedMembersMembership.Add(UserID, InMembership);
	}
	ProcessMemberStatusChanges(ReceivedMembersMembership);
	//CheckDirectConnectionValidity();
}

void MatrixRoomSession::InviteUser(const FString& UserID)
{
	if (p_Client != nullptr)
		if (GetAvailableInviteCount() > 0)
			p_Client->GetSession().InviteUserToRoom(m_RoomID, UserID);
}

void MatrixRoomSession::SendMessage(const FString& Message, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	if (p_Client != nullptr)
		p_Client->GetSession().SendMessage(m_RoomID, Message, TheDelegate);
}

//void MatrixRoomSession::SendCallInvite(FString CallID)
//{
//	// Remove bIsDirect from this
//	if (p_Client != nullptr && p_Client->GetSession().IsRoomDirectConnection(m_RoomID))
//	{
//		p_Client->GetSession().SendCallInvite(m_RoomID, CallID);
//	}
//}

//void MatrixRoomSession::OnCallInviteCallback(bool bWasSuccessful, FString _CallID)
//{
//
//}


//void MatrixRoomSession::SendCallAnswer(FString CallID)
//{
//	if (p_Client != nullptr)
//	{
//		p_Client->GetSession().SendCallAnswer(m_RoomID, CallID);
//	}
//}

//void MatrixRoomSession::SendCallHangup(FString CallID)
//{
//	if (p_Client != nullptr)
//	{
//		p_Client->GetSession().SendCallHangup(m_RoomID, CallID);
//	}
//}

EMatrixGetMessagesStatus MatrixRoomSession::GetPastMessages()
{
	if (p_Client != nullptr)
	{
		if (!bHasFetchedAllPastMessages && !bIsPastMessagesRequestProcessing)
		{
			bIsPastMessagesRequestProcessing = true;
			p_Client->GetSession().GetPastMessages(m_RoomID, m_StartingBatchToken);
			return EMatrixGetMessagesStatus::Processing;
		}
		
		if (bIsPastMessagesRequestProcessing)
			return EMatrixGetMessagesStatus::Processing;
		
		if (bHasFetchedAllPastMessages)
			return EMatrixGetMessagesStatus::NoMoreMessagesToFetch;
	
		return EMatrixGetMessagesStatus::ClientError;
	}
	else
		return EMatrixGetMessagesStatus::ClientError;
}

void MatrixRoomSession::OnGetMessagesResponse(const MatrixGetMessagesResponse& Response)
{
	if (Response.EndToken == Response.StartToken)
		bHasFetchedAllPastMessages = true;

	bIsPastMessagesRequestProcessing = false;

	m_StartingBatchToken = Response.EndToken;

	FMatrixMesssageEvents OldEvents;


	if (Response.RoomEvents.EventOrder.Num() == 0)
	{
		//bHasFetchedAllPastMessages = true;
		d_EventOnPastMessagesReceived.Broadcast(OldEvents);
		return;
	}
	
	//TODO: reverse the indices

	for (int32 Index = Response.RoomEvents.EventOrder.Num() - 1 ; Index >= 0; Index --)
	{
		EJoinedRoomEventType EventType = Response.RoomEvents.EventOrder[Index].Key;
		int32 EventIndex = Response.RoomEvents.EventOrder[Index].Value;

		if (EventType == EJoinedRoomEventType::CallInvite)
		{
			OldEvents.CallInvites.Add(Response.RoomEvents.CallInvites[EventIndex]);
			OldEvents.EventOrder.Add(TPair<EJoinedRoomEventType, int32>(EventType, OldEvents.CallInvites.Num() -1));
		}
		else if (EventType == EJoinedRoomEventType::CallAnswer)
		{
			OldEvents.CallAnswers.Add(Response.RoomEvents.CallAnswers[EventIndex]);
			OldEvents.EventOrder.Add(TPair<EJoinedRoomEventType, int32>(EventType, OldEvents.CallAnswers.Num() - 1));
		}
		else if (EventType == EJoinedRoomEventType::CallHangup)
		{
			OldEvents.CallHangUps.Add(Response.RoomEvents.CallHangUps[EventIndex]);
			OldEvents.EventOrder.Add(TPair<EJoinedRoomEventType, int32>(EventType, OldEvents.CallHangUps.Num() - 1));
		}
		else if (EventType == EJoinedRoomEventType::Message)
		{
			OldEvents.Messages.Add(Response.RoomEvents.Messages[EventIndex]);
			OldEvents.EventOrder.Add(TPair<EJoinedRoomEventType, int32>(EventType, OldEvents.Messages.Num() - 1));
		}
	}

	//
	TArray<FMatrixMesssageEvents> UpdatedMessageEvents;
	UpdatedMessageEvents.Add(OldEvents);
	UpdatedMessageEvents.Append(m_MessageEvents);

	m_MessageEvents = UpdatedMessageEvents;
	
	d_EventOnPastMessagesReceived.Broadcast(OldEvents);	
}

void MatrixRoomSession::AddToSyncBuffer(const FMatrixJoinedRoomSync& SyncObject)
{
	if (!SyncObject.RoomEvents.RoomName.IsEmpty())
	{
		m_RoomName = SyncObject.RoomEvents.RoomName;
		d_EventOnRoomNameChanged.Broadcast(m_RoomName);
	}

	if (SyncObject.RoomEvents.Admins.Num() > 0)
	{
		m_Admins = SyncObject.RoomEvents.Admins;
		d_EventOnAdminsReceived.Broadcast();
	}

	if (SyncObject.RoomEvents.MemberStatus.Num() > 0)
	{
		ProcessMemberStatusChanges(SyncObject.RoomEvents.MemberStatus);
	}

	d_EventOnUsersTyping.Broadcast(SyncObject.RoomEvents.TypingUsers);

	if (!bAreTokensInitialized)
	{
		m_StartingBatchToken = SyncObject.PrevBatch;
		m_PreviousBatchToken = SyncObject.PrevBatch;
		bAreTokensInitialized = true;
	}
	else
	{
		m_PreviousBatchToken = SyncObject.PrevBatch;
	}

	if (SyncObject.RoomEvents.FullyReadReceipt != "")
	{
		m_FullyReadMarker = SyncObject.RoomEvents.FullyReadReceipt;
	}

	if (SyncObject.RoomEvents.EventOrder.Num() > 0)
	{
		FMatrixMesssageEvents NewEvents;
		NewEvents.CallAnswers = SyncObject.RoomEvents.CallAnswers;
		NewEvents.CallHangUps = SyncObject.RoomEvents.CallHangUps;
		NewEvents.CallInvites = SyncObject.RoomEvents.CallInvites;
		NewEvents.EventOrder = SyncObject.RoomEvents.EventOrder;
		NewEvents.Messages = SyncObject.RoomEvents.Messages;

		// TODO: check if the existing events have self messages
		FString SelfEventID = "";
		if (p_Client != nullptr)
		{
			FString LoggedInUsername = p_Client->GetSession().GetLoggedInUserID();

			for (auto EventOrderPair : NewEvents.EventOrder)
			{
				if (EventOrderPair.Key == EJoinedRoomEventType::CallAnswer)
				{
					if (LoggedInUsername == NewEvents.CallAnswers[EventOrderPair.Value].Sender)
					{
						SelfEventID = NewEvents.CallAnswers[EventOrderPair.Value].EventID;
					}
				}
				else if (EventOrderPair.Key == EJoinedRoomEventType::CallInvite)
				{
					if (LoggedInUsername == NewEvents.CallInvites[EventOrderPair.Value].Sender)
					{
						SelfEventID = NewEvents.CallInvites[EventOrderPair.Value].EventID;
					}
				}
				else if (EventOrderPair.Key == EJoinedRoomEventType::CallHangup)
				{
					if (LoggedInUsername == NewEvents.CallHangUps[EventOrderPair.Value].Sender)
					{
						SelfEventID = NewEvents.CallHangUps[EventOrderPair.Value].EventID;
					}
				}
				else
				{
					if (LoggedInUsername == NewEvents.Messages[EventOrderPair.Value].Sender)
					{
						SelfEventID = NewEvents.Messages[EventOrderPair.Value].EventID;
					}
				}
			}
		}

		m_MessageEvents.Add(NewEvents);
		d_EventOnNewMessagesReceived.Broadcast(NewEvents);

		if (SelfEventID != "" && !bIsInitialSync)
		{
			d_EventOnSelfMessageReceived.Broadcast(SelfEventID);
			UpdateFullyReadMarker(SelfEventID);
		}
	}	
	
	FString LastEventID = GetEventIDOfLatestEvent();
	if (m_FullyReadMarker != LastEventID && m_FullyReadMarker_Local != LastEventID)
	{
		SetHasNewUnreadNotification(true);
	}
	else
	{
		SetHasNewUnreadNotification(false);
	}

	if (bIsInitialSync)
	{
		bIsInitialSync = false;
	}

}

void MatrixRoomSession::SetHasNewUnreadNotification(bool bInHasNewNotifications)
{
	// only send events if the notifcation value is different 
	if (bHasUnreadNotifications != bInHasNewNotifications)
	{
		bHasUnreadNotifications = bInHasNewNotifications;

		if (p_Client != nullptr)
		{
			p_Client->GetSession().SendChatEventNotification(m_RoomID, bHasUnreadNotifications, bIsDirect);
		}

		d_EventOnNewUnreadEvents.Broadcast(bHasUnreadNotifications);
	}	
}

TArray<FString> MatrixRoomSession::GetAdmins() const
{
	return m_Admins;
}

void MatrixRoomSession::KickUser(const FString& UserID, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	if (p_Client != nullptr)
	{
		if (IsAdminOfThisRoom())
		{
			p_Client->GetSession().KickUserFromRoom(m_RoomID, UserID, TheDelegate);
		}
		else
		{
			TheDelegate.ExecuteIfBound(EMatrixCoreError::UNAUTHORIZED);
		}
	}
	else
	{
		TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
	}
}

void MatrixRoomSession::UpdateRoomName(const FString& NewName, const MatrixNativeAPI::FOnMatrixAPIResponseDelegate& TheDelegate)
{
	if (p_Client != nullptr)
	{
		if (IsAdminOfThisRoom())
		{
			p_Client->GetSession().UpdateRoomName(m_RoomID, NewName, TheDelegate);
		}
		else
		{
			TheDelegate.ExecuteIfBound(EMatrixCoreError::UNAUTHORIZED);
		}
	}
	else
	{
		TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
	}
}

void MatrixRoomSession::ProcessMemberStatusChanges(const TMap<FString, EMatrixMembership>& NewMemberStatus)
{
	/*for (auto MemberPair : NewMemberStatus)
	{
		if (MemberStatus.Contains(MemberPair.Key))
		{
			EMatrixMembership Membership = *MemberStatus.Find(MemberPair.Key);

			if (Membership != MemberPair.Value)
			{
				if (MemberPair.Value == EMatrixMembership::Leave)
				{
					MemberStatus.Remove(MemberPair.Key);
				}
				else
				{
					MemberStatus.Add(MemberPair);
				}

				EventOnMembershipChangedForMember.Broadcast(MemberPair.Key, MemberPair.Value);

			}
		}
		else
		{
			if (MemberPair.Value != EMatrixMembership::Leave)
			{
				MemberStatus.Add(MemberPair);
				EventOnMembershipChangedForMember.Broadcast(MemberPair.Key, MemberPair.Value);
			}
		}
	}*/

	// TODO: on call check direct connection validity if things changes

	for (auto MemberPair : NewMemberStatus)
	{
		if (m_MemberStatus.Contains(MemberPair.Key))
		{
			EMatrixMembership Membership = *m_MemberStatus.Find(MemberPair.Key);

			if (Membership != MemberPair.Value)
			{
				m_MemberStatus.Add(MemberPair);
				d_EventOnMembershipChangedForMember.Broadcast(MemberPair.Key, MemberPair.Value);
			}
		}
		else
		{
			m_MemberStatus.Add(MemberPair);
			d_EventOnMembershipChangedForMember.Broadcast(MemberPair.Key, MemberPair.Value);
		}
	}

	CheckDirectConnectionValidity();
}

TArray<FString> MatrixRoomSession::GetMemberListForMembership(EMatrixMembership Membership)
{
	TArray<FString> FoundList;
	for (auto MemberPair : m_MemberStatus)
	{
		if (MemberPair.Value == Membership)
		{
			FoundList.Add(MemberPair.Key);
		}
	}

	return FoundList;
}

bool MatrixRoomSession::IsAdminOfThisRoom() const
{
	if (p_Client != nullptr)
		return m_Admins.Contains(p_Client->GetSession().GetLoggedInUserID());

	else
		return false;
}

TArray<FMatrixMesssageEvents> MatrixRoomSession::GetMessageEvents() const
{
	return m_MessageEvents;
}

int32 MatrixRoomSession::GetAvailableInviteCount()
{
	if (!bIsDirect)
	{
		int32 TotalMembers = GetMemberListForMembership(EMatrixMembership::Invite).Num() + GetMemberListForMembership(EMatrixMembership::Join).Num();
		return MATRIX_TEAM_MAX_USERS - TotalMembers;
	}

	return -1;
}

void MatrixRoomSession::SendTypingNotification(bool bIsTyping) const
{
	if (p_Client != nullptr)
		p_Client->GetSession().SendTypingNotification(m_RoomID, bIsTyping);
}

FString MatrixRoomSession::GetEventIDOfLatestEvent()
{
	if (m_MessageEvents.Num() > 0)
	{
		FMatrixMesssageEvents Events = m_MessageEvents.Last();
		if (Events.EventOrder.Num() > 0)
		{
			TPair<EJoinedRoomEventType, int32> LastEventPair = Events.EventOrder.Last();
			if (LastEventPair.Key == EJoinedRoomEventType::CallAnswer)
				return Events.CallAnswers[LastEventPair.Value].EventID;
			
			if (LastEventPair.Key == EJoinedRoomEventType::CallHangup)
				return Events.CallHangUps[LastEventPair.Value].EventID;
			
			if (LastEventPair.Key == EJoinedRoomEventType::CallInvite)
				return Events.CallInvites[LastEventPair.Value].EventID;
			
			if (LastEventPair.Key == EJoinedRoomEventType::Message)
				return Events.Messages[LastEventPair.Value].EventID;
		}
	}

	return "";
}


void MatrixRoomSession::UpdateFullyReadMarker()
{
	if (p_Client != nullptr)
	{
		FString LastEventID = GetEventIDOfLatestEvent();
		if (LastEventID != "" && m_FullyReadMarker != LastEventID)
		{
			m_FullyReadMarker_Local = LastEventID;
			p_Client->GetSession().UpdateFullReadMarkerForRoom(m_RoomID, LastEventID);
		}	
	}
}

void MatrixRoomSession::UpdateFullyReadMarker(const FString& InEventID)
{
	if (p_Client != nullptr)
	{
		if (InEventID != "" && m_FullyReadMarker_Local != InEventID)
		{
			m_FullyReadMarker_Local = InEventID;
			p_Client->GetSession().UpdateFullReadMarkerForRoom(m_RoomID, InEventID);
		}
	}
}


void MatrixRoomSession::CheckDirectConnectionValidity()
{
	if (bIsDirect && p_Client != nullptr)
	{
		int32 LeftMembersCount = GetMemberListForMembership(EMatrixMembership::Leave).Num();
		int32 JoinedMembersCount = GetMemberListForMembership(EMatrixMembership::Join).Num();
		// int32 TotalMembers = InvitedMembersCount  + JoinedMembersCount;


		if (JoinedMembersCount == 2)
		{
			// friend request accepted
			p_Client->GetSession().ConfirmFriendRoom(m_RoomID);
		}
		else if (JoinedMembersCount == 1 && LeftMembersCount == 1)
		{
			// leave the room friend request cancelled
			p_Client->GetSession().LeaveRoom(m_RoomID, bIsDirect);
		}

	}
}
