#pragma once
#include "CoreMinimal.h"
#include "ChannelId.h"


struct FTesserconTokens
{
	FString AccessToken;
	FString RefreshToken;
	FDateTime ExpiresIn;
};

UENUM()
enum class EUserRole : uint8
{
	Exhibitor,
	Attendee
};


enum class EChatType : uint8
{
	Single,
	Group
};

enum class ETesserconLoginState : uint8
{
	LoggedOut,
	LogginIn,
	LoggingOut,
	LoggedIn,
	LoggedInNoActiveEvents
};

UENUM()
enum class ETesserconGender : uint8
{
	Male,
	Female,
	Other
};

enum class EEventSubscriptionStatus : uint8
{
	Unknown,
	None,
	Active
};


struct FTesserconEventOrganiser
{
	FString OrganiserName;
	FString OrganiserLogoUrl;
	FString OrganiserContactUrl;
};

//TODO: use inheritance for event detail versions
struct FTesserconEventsList
{
	FString EventID;
	FString EventBannerURL;
};

struct FTesserconEvent:FTesserconEventsList
{
	FString EventTitle;
	FString MatrixPublicRoomID;
	FString Description;
	FDateTime StartTime;
	FDateTime EndTime;
	FString EventCompanyID;
	FString VariantName;
	FString VariantURL;
	EUserRole UserType;
};

struct FTesserconBoothList
{
	FString BoothID;
	FString BoothName;
	FString BoothLocation;
	FString BoothBannerURL;
	FVector BoothMapLocation;
};

struct FTesserconUserInfo
{
	FString Username;
	FString Email;
	FString FirstName;
	FString LastName;
	FString ProfileImageURL;
	FString Designation;
	FString Phone;
	FString Website;
	FString CompanyName;
	FString CompanyLogoURL;
	FString Address;
	FString Bio;
	EUserRole UserRole;
};

struct FTesserconBooth :FTesserconBoothList
{
	FString BoothDescription;
	TArray<FTesserconUserInfo> BoothExhibitors;
	FString CompanyEmail;
	FString CompanyPhone;
};

enum class ETesserconCallStatus : uint8
{
	Available,
	OnCall,
	Processing,
	Dialing,
	Switching,
	Connecting
};

enum class ECallType: uint8
{
	Team,
	Individual
};

struct FTesserconCallState
{
	FString RoomID;
	FString CallID;
	ECallType CallType;
	ETesserconCallStatus CallStatus;

	FTesserconCallState()
	{	}

	FTesserconCallState(ETesserconCallStatus InCallStatus) : CallStatus(InCallStatus)
	{	}

	FTesserconCallState(const FString& InRoomID, const FString& InCallID, ECallType InCallType, ETesserconCallStatus InCallStatus) :
		RoomID(InRoomID), CallID(InCallID), CallType(InCallType), CallStatus(InCallStatus)
	{	}
};

enum class EVivoxChannelKey: uint8
{
	NoChannel,
	NearByChannel,
	VoiceCallChannel,
	GlobalTextOnlyChannel
};

struct VivoxChannelProperties
{
	ChannelType Type;
	bool bShouldTransmitOnJoin;
	bool bConnectAudio;
	bool bConnectText;
};

template<class T>
class CachedProperty
{
public:
	explicit CachedProperty(T value)
	{
		m_dirty = false;
		m_value = value;
	}

	const T& GetValue() const
	{
		return m_value;
	}

	void SetValue(const T& value)
	{
		if (m_value != value)
		{
			m_value = value;
			m_dirty = true;
		}
	}

	void SetDirty(bool value)
	{
		m_dirty = value;
	}

	bool IsDirty() const
	{
		return m_dirty;
	}
protected:
	bool m_dirty;
	T m_value;
};

VivoxChannelProperties GetChannelPropertiesForChannelKey(EVivoxChannelKey Key);

template<typename TEnum>
static FORCEINLINE FString GetUEnumAsString(const FString& Name, TEnum Value, bool ShortName)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
		return FString("InvalidUEnum");

	if (ShortName)
		return enumPtr->GetNameStringByIndex((int64)Value);

	return enumPtr->GetNameByValue((int64)Value).ToString();
}

#define UEnumFullToString(Name, Value) GetUEnumAsString<Name>(#Name, Value, false)
#define UEnumShortToString(Name, Value) GetUEnumAsString<Name>(#Name, Value, true)

FString TESSERCONCORE_API GetUsernameFromMatrixUsername(const FString& MatrixUsername);
FString TESSERCONCORE_API GetMatrixFullUserIDForUsername(const FString& Username);