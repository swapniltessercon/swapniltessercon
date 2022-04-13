#pragma once
#include "MatrixError.h"
#include "TesserconCoreCommon.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMatrixCore, Log, All);

#define MATRIX_SERVER TEXT("https://comms.tessercon.com")
#define MATRIX_CALL_INVITE_LIFETIME 60000
#define MATRIX_TRANSATION_ID_LENGTH 6
#define MATRIX_CALL_ID_LENGTH 5
// TODO: users set to 5 can be changed upto 100
#define MATRIX_TEAM_MAX_USERS 10

// TODO: change the name to matrix call state
enum class EMatrixCallState : uint8
{
	Inviting,
	//Incomming,
	Dialing,
	Answering,
	OnGoing,
	Ended
};

enum class EMatrixCallEventType : uint8
{
	Invite,
	Answer,
	Hangup
	// candidates
};

FString MATRIXCORE_API ConvertMatrixCallStateToString(EMatrixCallState State);

enum class EMatrixPresence : uint8
{
	Online,
	Offline,
	Unavailable
};

enum class EMatrixMembership : uint8
{
	Join,
	Invite,
	Leave,
	Ban
};

enum class EMatrixCallHangUpReason : uint8
{
	None,
	InviteTimeOut
};

EMatrixCoreError ConvertStringToMatrixCoreError(const FString& ErrorCode);
EMatrixPresence ConvertStringToMatrixPresence(const FString& Status);
EMatrixMembership ConvertStringToMatrixMembership(const FString& Membership);
FString ConvertMatrixMembershipToString(EMatrixMembership Membership);

FString MATRIXCORE_API ConvertMatrixCallEventTypeToString(EMatrixCallEventType EventType);

struct FMatrixTextMessage
{
	FString Message;
	FDateTime TimeStamp;
	FString Sender;
	FString EventID;
};


struct FMatrixCallEvent
{
	FString Sender;
	FDateTime TimeStamp;
	FDateTime ExpireTime;
	FString CallID;
	FString EventID;
};

//struct FMatrixCallInvite
//{
//	FString Sender;
//	FDateTime TimeStamp;
//	FDateTime ExpireTime;
//	FString CallID;
//
//};
//
//struct FMatrixCallAnswer
//{
//	FString Sender;
//	FDateTime TimeStamp;
//	FString CallID;
//
//};
//
//struct FMatrixCallHangUp
//{
//	FString Sender;
//	FDateTime TimeStamp;
//	FString CallID;
//
//};

enum class EJoinedRoomEventType : uint8
{
	Message,
	CallInvite,
	CallHangup,
	CallAnswer
};

struct FMatrixJoinedRoomEvents
{
	TArray<TPair<EJoinedRoomEventType, int32>> EventOrder;
	TArray<FMatrixTextMessage> Messages;
	TArray<FMatrixCallEvent> CallInvites;
	TArray<FMatrixCallEvent> CallAnswers;
	TArray<FMatrixCallEvent> CallHangUps;

	// Not Included in the event order
	FString RoomName;
	TArray<FString> Admins;
	TArray<FString> TypingUsers;
	TMap<FString, EMatrixMembership> MemberStatus;
	FString FullyReadReceipt;
};

struct FMatrixJoinedRoomSync
{
	// state
	// power levels
	// messages
	FMatrixJoinedRoomEvents RoomEvents;
	FString RoomID;
	FString PrevBatch;
};

struct FMatrixInvitedRoom
{
	FString Sender;
	FString RoomID;
	FString Receiver;
	bool bIsDirect;
	FString Name;

	FMatrixInvitedRoom():Sender(""), RoomID(""), Receiver(""), bIsDirect(false), Name("")
	{

	};
};

// not required at this point of time
struct FMatrixLeftRoom
{
	//FString 
};

struct FMatrixRooms
{
	TArray<FMatrixJoinedRoomSync> JoinedRooms;
	TArray<FMatrixInvitedRoom> InvitedRooms;
	TArray<FString> LeftRooms;
};

//struct FMatrixPresence
//{
//	FString Sender;
//	EMatrixPresence Presence;
//	
//};

struct FMatrixAccountData
{
	TMap<FString, FString> DirectConnections;
	TArray<FString> BlockedUsers;
	bool bHasBlockedUserUpdate = false;
	bool bHasDirectConnectionsUpdate= false;
};


FString MATRIXCORE_API GenerateTransactionID(int IDLength);

enum class EMatrixUserRelation : uint8
{
	Friend,
	NotFriend,
	Blocked,
	RequestSent,
	RequestReceived
};

struct FMatrixMesssageEvents
{
	TArray<TPair<EJoinedRoomEventType, int32>> EventOrder;
	TArray<FMatrixTextMessage> Messages;
	TArray<FMatrixCallEvent> CallInvites;
	TArray<FMatrixCallEvent> CallAnswers;
	TArray<FMatrixCallEvent> CallHangUps;

};

enum class EMatrixGetMessagesStatus
{
	NoMoreMessagesToFetch,
	ClientError,
	Processing
};

// TODO: change this to EMatrixLoginState
enum class ELoginState : uint8
{
	LoggedOut,
	LogginIn,
	LoggedIn,
	LoggingOut
};