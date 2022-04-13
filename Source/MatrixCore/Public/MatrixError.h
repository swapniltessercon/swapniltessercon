#pragma once
#include "CoreMinimal.h"


//UENUM()
enum class EMatrixCoreError : uint8
{
	Success,

    // Forbidden access, e.g.joining a room without permission, failed login.
    FORBIDDEN,

    // The access token specified was not recognised.
    // An additional response parameter, soft_logout, might be present on the response for 401 HTTP status codes.See the soft logout section for more information.
    UNKNOWN_TOKEN,

    // No access token was specified for the request.
    MISSING_TOKEN,

    // Request contained valid JSON, but it was malformed in some way, e.g.missing required keys, invalid values for keys.
    BAD_JSON,

    // Request did not contain valid JSON.
    NOT_JSON,

    // No resource was found for this request.
    NOT_FOUND,

    // Too many requests have been sent in a short period of time.Wait a while then try again.
    LIMIT_EXCEEDED,

    //An unknown error has occurred.
    UNKNOWN,

    // Other error codes the client might encounter are :

    //The server did not understand the request.
    UNRECOGNIZED,

    // The request was not correctly authorized.Usually due to login failures.
    UNAUTHORIZED,

    // The user ID associated with the request has been deactivated.Typically for endpoints that prove authentication, such as / login.
    USER_DEACTIVATED,

    // Encountered when trying to register a user ID which has been taken.
    USER_IN_USE,

    // Encountered when trying to register a user ID which is not valid.
    INVALID_USERNAME,

    // Sent when the room alias given to the createRoom API is already in use.
    ROOM_IN_USE,

    // Sent when the initial state given to the createRoom API is invalid.
    INVALID_ROOM_STATE,

    //Sent when a threepid given to an API cannot be used because the same threepid is already in use.
    THREEPID_IN_USE,

    // Sent when a threepid given to an API cannot be used because no record matching the threepid was found.
    THREEPID_NOT_FOUND,

    // Authentication could not be performed on the third party identifier.
    THREEPID_AUTH_FAILED,

    // The server does not permit this third party identifier.This may happen if the server only permits, for example, email addresses from a particular domain.
    THREEPID_DENIED,

    // The client's request used a third party server, eg. identity server, that this server does not trust.
    SERVER_NOT_TRUSTED,

    // The client's request to create a room used a room version that the server does not support.
    UNSUPPORTED_ROOM_VERSION,

    // The client attempted to join a room that has a version the server does not support.Inspect the room_version property of the error response for the room's version.
    INCOMPATIBLE_ROOM_VERSION,

    // The state change requested cannot be performed, such as attempting to unban a user who is not banned.
    BAD_STATE,

    // The room or resource does not permit guests to access it.
    GUEST_ACCESS_FORBIDDEN,

    // A Captcha is required to complete the request.
    CAPTCHA_NEEDED,

    // The Captcha provided did not match what was expected.
    CAPTCHA_INVALID,

    // A required parameter was missing from the request.
    MISSING_PARAM,

    // A parameter that was specified has the wrong value.For example, the server expected an integerand instead received a string.
    INVALID_PARAM,

    // The request or entity was too large.
    TOO_LARGE,

    // The resource being requested is reserved by an application service, or the application service making the request has not created the resource. 
    EXCLUSIVE,

    //The request cannot be completed because the homeserver has reached a resource limit imposed on it.For example, a homeserver held in a shared hosting environment may reach a resource limit if it starts using too much memory or disk space.The error MUST have an admin_contact field to provide the user receiving the error a place to reach out to.Typically, this error will appear on routes which attempt to modify state(eg : sending messages, account data, etc) and not routes which only read state(eg : / sync, get account data, etc).
    RESOURCE_LIMIT_EXCEEDED,

    // The user is unable to reject an invite to join the server notices room.See the Server Notices module for more information.
    CANNOT_LEAVE_SERVER_NOTICE_ROOM
};







