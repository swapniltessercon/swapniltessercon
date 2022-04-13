#include "MatrixCommon.h"

#include <string>


DEFINE_LOG_CATEGORY(LogMatrixCore);

TMap<FString, EMatrixCoreError>& GetMatrixCoreErrorMap()
{
    static TMap<FString, EMatrixCoreError> Map;
    static bool bIsCalled = false;
    if (!bIsCalled)
    {
        // Forbidden access, e.g.joining a room without permission, failed login.
        Map.Add("M_FORBIDDEN", EMatrixCoreError::FORBIDDEN);

        // The access token specified was not recognised.
        // An additional response parameter, soft_logout, might be present on the response for 401 HTTP status codes.See the soft logout section for more information. 
        Map.Add("M_UNKNOWN_TOKEN", EMatrixCoreError::UNKNOWN_TOKEN);

        // No access token was specified for the request.
        Map.Add("M_MISSING_TOKEN", EMatrixCoreError::MISSING_TOKEN);

        // Request contained valid JSON, but it was malformed in some way, e.g.missing required keys, invalid values for keys.
        Map.Add("M_BAD_JSON", EMatrixCoreError::BAD_JSON);

        // Request did not contain valid JSON.
        Map.Add("M_NOT_JSON", EMatrixCoreError::NOT_JSON);

        // No resource was found for this request.
        Map.Add("M_NOT_FOUND", EMatrixCoreError::NOT_FOUND);

        // Too many requests have been sent in a short period of time.Wait a while then try again.
        Map.Add("M_LIMIT_EXCEEDED", EMatrixCoreError::LIMIT_EXCEEDED);

        //An unknown error has occurred.
        Map.Add("M_UNKNOWN", EMatrixCoreError::UNKNOWN);

        // Other error codes the client might encounter are :

        //The server did not understand the request.
        Map.Add("M_UNRECOGNIZED", EMatrixCoreError::UNRECOGNIZED);

        // The request was not correctly authorized.Usually due to login failures.
        Map.Add("M_UNAUTHORIZED", EMatrixCoreError::UNAUTHORIZED);

        // The user ID associated with the request has been deactivated.Typically for endpoints that prove authentication, such as / login.
        Map.Add("M_USER_DEACTIVATED", EMatrixCoreError::USER_DEACTIVATED);

        // Encountered when trying to register a user ID which has been taken.
        Map.Add("M_USER_IN_USE", EMatrixCoreError::USER_IN_USE);

        // Encountered when trying to register a user ID which is not valid.
        Map.Add("M_INVALID_USERNAME", EMatrixCoreError::INVALID_USERNAME);

        // Sent when the room alias given to the createRoom API is already in use.
        Map.Add("M_ROOM_IN_USE", EMatrixCoreError::ROOM_IN_USE);

        // Sent when the initial state given to the createRoom API is invalid.
        Map.Add("M_INVALID_ROOM_STATE", EMatrixCoreError::INVALID_ROOM_STATE);

        //Sent when a threepid given to an API cannot be used because the same threepid is already in use.
        Map.Add("M_THREEPID_IN_USE", EMatrixCoreError::THREEPID_IN_USE);

        // Sent when a threepid given to an API cannot be used because no record matching the threepid was found.
        Map.Add("M_THREEPID_NOT_FOUND", EMatrixCoreError::THREEPID_NOT_FOUND);

        // Authentication could not be performed on the third party identifier.
        Map.Add("M_THREEPID_AUTH_FAILED", EMatrixCoreError::THREEPID_AUTH_FAILED);

        // The server does not permit this third party identifier.This may happen if the server only permits, for example, email addresses from a particular domain.
        Map.Add("M_THREEPID_DENIED", EMatrixCoreError::THREEPID_DENIED);

        // The client's request used a third party server, eg. identity server, that this server does not trust.
        Map.Add("M_SERVER_NOT_TRUSTED", EMatrixCoreError::SERVER_NOT_TRUSTED);

        // The client's request to create a room used a room version that the server does not support.
        Map.Add("M_UNSUPPORTED_ROOM_VERSION", EMatrixCoreError::UNSUPPORTED_ROOM_VERSION);

        // The client attempted to join a room that has a version the server does not support.Inspect the room_version property of the error response for the room's version.
        Map.Add("M_INCOMPATIBLE_ROOM_VERSION", EMatrixCoreError::INCOMPATIBLE_ROOM_VERSION);

        // The state change requested cannot be performed, such as attempting to unban a user who is not banned.
        Map.Add("M_BAD_STATE", EMatrixCoreError::BAD_STATE);

        // The room or resource does not permit guests to access it.
        Map.Add("M_GUEST_ACCESS_FORBIDDEN", EMatrixCoreError::GUEST_ACCESS_FORBIDDEN);
        
        // A Captcha is required to complete the request.
        Map.Add("M_CAPTCHA_NEEDED", EMatrixCoreError::CAPTCHA_NEEDED);
        
        // The Captcha provided did not match what was expected.
        Map.Add("M_CAPTCHA_INVALID", EMatrixCoreError::CAPTCHA_INVALID);

        // A required parameter was missing from the request.
        Map.Add("M_MISSING_PARAM", EMatrixCoreError::MISSING_PARAM);

        // A parameter that was specified has the wrong value.For example, the server expected an integerand instead received a string.
        Map.Add("M_INVALID_PARAM", EMatrixCoreError::INVALID_PARAM);
        
        // The request or entity was too large.
        Map.Add("M_TOO_LARGE", EMatrixCoreError::TOO_LARGE);

        // The resource being requested is reserved by an application service, or the application service making the request has not created the resource. 
        Map.Add("M_EXCLUSIVE", EMatrixCoreError::EXCLUSIVE);

        /*The request cannot be completed because the homeserver has reached a resource limit imposed on it.For example, 
        a homeserver held in a shared hosting environment may reach a resource limit if it starts using too much memory or disk space.
        The error MUST have an admin_contact field to provide the user receiving the error a place to reach out to.Typically, this error 
        will appear on routes which attempt to modify state(eg : sending messages, account data, etc) and not routes which only read state(eg : / sync, get account data, etc).*/
        Map.Add("M_RESOURCE_LIMIT_EXCEEDED", EMatrixCoreError::RESOURCE_LIMIT_EXCEEDED);

        // The user is unable to reject an invite to join the server notices room.See the Server Notices module for more information.
        Map.Add("M_CANNOT_LEAVE_SERVER_NOTICE_ROOM", EMatrixCoreError::CANNOT_LEAVE_SERVER_NOTICE_ROOM);

        bIsCalled = true;
    }
    return Map;
}

EMatrixCoreError ConvertStringToMatrixCoreError(const FString& ErrorCode)
{
    TMap<FString, EMatrixCoreError>& ErrorMap = GetMatrixCoreErrorMap();

    if (ErrorMap.Contains(ErrorCode))
        return ErrorMap[ErrorCode];
    
    /*if (ErrorCode == "M_FORBIDDEN") return EMatrixCoreError::FORBIDDEN;

    
    if (ErrorCode == "M_UNKNOWN_TOKEN") return EMatrixCoreError::UNKNOWN_TOKEN;

    
    if (ErrorCode == "M_MISSING_TOKEN") return EMatrixCoreError::MISSING_TOKEN;

    
    if (ErrorCode == "M_BAD_JSON") return EMatrixCoreError::BAD_JSON;

    
    if (ErrorCode == "M_NOT_JSON") return EMatrixCoreError::NOT_JSON;

    
    if (ErrorCode == "M_NOT_FOUND") return EMatrixCoreError::NOT_FOUND;

    
    if (ErrorCode == "M_LIMIT_EXCEEDED") return EMatrixCoreError::LIMIT_EXCEEDED;

    
    if (ErrorCode == "M_UNKNOWN") return EMatrixCoreError::UNKNOWN;

    
    if (ErrorCode == "M_UNRECOGNIZED") return EMatrixCoreError::UNRECOGNIZED;

    
    if (ErrorCode == "M_UNAUTHORIZED") return EMatrixCoreError::UNAUTHORIZED;

    
    if (ErrorCode == "M_USER_DEACTIVATED") return EMatrixCoreError::USER_DEACTIVATED;

    
    if (ErrorCode == "M_USER_IN_USE") return EMatrixCoreError::USER_IN_USE;

    
    if (ErrorCode == "M_INVALID_USERNAME") return EMatrixCoreError::INVALID_USERNAME;

    
    if (ErrorCode == "M_ROOM_IN_USE") return EMatrixCoreError::ROOM_IN_USE;

    
    if (ErrorCode == "M_INVALID_ROOM_STATE") return EMatrixCoreError::INVALID_ROOM_STATE;

    
    if (ErrorCode == "M_THREEPID_IN_USE") return EMatrixCoreError::THREEPID_IN_USE;

    
    if (ErrorCode == "M_THREEPID_NOT_FOUND") return EMatrixCoreError::THREEPID_NOT_FOUND;

    
    if (ErrorCode == "M_THREEPID_AUTH_FAILED") return EMatrixCoreError::THREEPID_AUTH_FAILED;

    
    if (ErrorCode == "M_THREEPID_DENIED") return EMatrixCoreError::THREEPID_DENIED;

    
    if (ErrorCode == "M_SERVER_NOT_TRUSTED") return EMatrixCoreError::SERVER_NOT_TRUSTED;

    
    if (ErrorCode == "M_UNSUPPORTED_ROOM_VERSION") return EMatrixCoreError::UNSUPPORTED_ROOM_VERSION;

    
    if (ErrorCode == "M_INCOMPATIBLE_ROOM_VERSION") return EMatrixCoreError::INCOMPATIBLE_ROOM_VERSION;
    
    
    if (ErrorCode == "M_BAD_STATE") return EMatrixCoreError::BAD_STATE;

    
    if (ErrorCode == "M_GUEST_ACCESS_FORBIDDEN") return EMatrixCoreError::GUEST_ACCESS_FORBIDDEN;

    
    if (ErrorCode == "M_CAPTCHA_NEEDED") return EMatrixCoreError::CAPTCHA_NEEDED;

    
    if (ErrorCode == "M_CAPTCHA_INVALID") return EMatrixCoreError::CAPTCHA_INVALID;


    
    if (ErrorCode == "M_MISSING_PARAM") return EMatrixCoreError::MISSING_PARAM;

    
    if (ErrorCode == "M_INVALID_PARAM") return EMatrixCoreError::INVALID_PARAM;

    
    if (ErrorCode == "M_TOO_LARGE") return EMatrixCoreError::TOO_LARGE;

    
    if (ErrorCode == "M_EXCLUSIVE") return EMatrixCoreError::EXCLUSIVE;

    
    if (ErrorCode == "M_RESOURCE_LIMIT_EXCEEDED") return EMatrixCoreError::RESOURCE_LIMIT_EXCEEDED;

    
    if (ErrorCode == "M_CANNOT_LEAVE_SERVER_NOTICE_ROOM") return EMatrixCoreError::CANNOT_LEAVE_SERVER_NOTICE_ROOM;*/
    
    // The error is unkown
    return EMatrixCoreError::UNKNOWN;
}


EMatrixPresence ConvertStringToMatrixPresence(const FString& Status)
{
    if (Status == "online") return EMatrixPresence::Online;

    if (Status == "unavailable") return EMatrixPresence::Unavailable;
    
    return EMatrixPresence::Offline;
}

EMatrixMembership ConvertStringToMatrixMembership(const FString& Membership)
{
    if (Membership == "join") return EMatrixMembership::Join;
    
    if (Membership == "invite") return EMatrixMembership::Invite;
    
    if (Membership == "leave") return EMatrixMembership::Leave;
    
    return EMatrixMembership::Ban;    
}

FString ConvertMatrixMembershipToString(EMatrixMembership Membership)
{
    if (Membership == EMatrixMembership::Join) return "join";
    
    if (Membership == EMatrixMembership::Leave) return "leave";
    
    if (Membership == EMatrixMembership::Invite) return "invite";
    
    return "ban";
}

FString GenerateTransactionID(int IDLength)
{
    // All possible characters of my transactionID
    std::string Template = "abcdefghijklmnopqrstuvwxyzABCD"
        "EFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int TemplateLength = Template.length();
   
    std::string TransactionID;

    for (int i = 1; i <= IDLength; i++)
        TransactionID.push_back(Template[rand() % TemplateLength]);

    FString Result(TransactionID.c_str());
    return Result;
}

FString ConvertMatrixCallStateToString(EMatrixCallState State)
{
    switch (State)
    {
    case EMatrixCallState::Inviting:
        return "Inviting";
    case EMatrixCallState::Dialing:
        return "Dailing";
    case EMatrixCallState::Answering:
        return "Answering";
    case EMatrixCallState::OnGoing:
        return "Connected";
    case EMatrixCallState::Ended:
        return "Ended";
    default:
        return "";
    }
}

FString MATRIXCORE_API ConvertMatrixCallEventTypeToString(EMatrixCallEventType EventType)
{
    switch (EventType)
    {
    case EMatrixCallEventType::Answer:
        return "Answer";
    case EMatrixCallEventType::Invite:
        return "Invite";
    case EMatrixCallEventType::Hangup:
        return "Hangup";
    default:
        return "";
    }
}
