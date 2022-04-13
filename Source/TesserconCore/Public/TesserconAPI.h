#pragma once

#include "CoreMinimal.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "TesserconCoreErrors.h"
#include "TesserconResponses.h"


class TesserconAPI
{

public:
	//For Login
	DECLARE_DELEGATE_ThreeParams(FDelegateLoginResponse, ETesserconError, const FTesserconLoginResponse&, const FString&)
	FDelegateLoginResponse d_OnLoginResponseReceived;
	
	//List Info
	DECLARE_DELEGATE_TwoParams(FOnSubscribedEventsListForUserResponseDelegate, ETesserconError,TArray<FTesserconEventsList>);
	void GetEventsSubscribedListForUser(const FString& AccessToken,  const FOnSubscribedEventsListForUserResponseDelegate& TheDelegate = FOnSubscribedEventsListForUserResponseDelegate());

	DECLARE_DELEGATE_TwoParams(FOnBoothListForEventResponseDelegate, ETesserconError, TArray<FTesserconBoothList>);
	void GetBoothListForEvent(const FString& AccessToken, const FString& EventID,const FOnBoothListForEventResponseDelegate& TheDelegate = FOnBoothListForEventResponseDelegate());

	//EventUserInfo
	DECLARE_DELEGATE_TwoParams(FOnEventInfoForUserResponseDelegate, const ETesserconError&, const FTesserconEvent&);
	void GetEventInfoForUser(const FString& AccessToken, const FString& EventID, const FOnEventInfoForUserResponseDelegate& TheDelegate = FOnEventInfoForUserResponseDelegate());

	DECLARE_DELEGATE_TwoParams(FOnBoothInfoForEventResponseDelegate, const ETesserconError&, const FTesserconBooth&);
	void GetBoothInfoForEvent(const FString& AccessToken, const FString& BoothID, const FOnBoothInfoForEventResponseDelegate& TheDelegate = FOnBoothInfoForEventResponseDelegate());

	//EventOrganiserInfo
	DECLARE_DELEGATE_TwoParams(FOnEventOrganiserResponseDelegate, const ETesserconError&, const FTesserconEventOrganiser&);
	void GetOrganiserForEvent(const FString& AccessToken, const FString& EventID, const FOnEventOrganiserResponseDelegate& TheDelegate = FOnEventOrganiserResponseDelegate());

	//EventSponsorInfo
	DECLARE_DELEGATE_TwoParams(FOnEventSponsorsResponseDelegate, const ETesserconError&, const TArray<FString>&);
	void GetSponsorsForEvent(const FString& AccessToken, const FString& EventID, const FOnEventSponsorsResponseDelegate& TheDelegate = FOnEventSponsorsResponseDelegate());
	
	//Mail Auth Responce
	DECLARE_DELEGATE_TwoParams(FOnForgotPasswordForUserResponseDelegate, ETesserconError, const FString& ErrorMessgae);
	void ForgotPassword(const FString& EmailID, const FOnForgotPasswordForUserResponseDelegate& TheDelegate = FOnForgotPasswordForUserResponseDelegate());

	//GetLoggedInUserInfo
	DECLARE_DELEGATE_TwoParams(FOnLoggedinUserInfoResponseDelegate, ETesserconError, const FTesserconUserInfo&)
	void GetLoggedinUserInfo(const FString& AccessToken, const FOnLoggedinUserInfoResponseDelegate& TheDelegate = FOnLoggedinUserInfoResponseDelegate());

	DECLARE_DELEGATE_TwoParams(FOnUserInfoByUserIDResponseDelegate, ETesserconError, FTesserconUserInfo);
	void GetUserInfoForUser(const FString& AccessToken, const FString& Username, const FOnUserInfoByUserIDResponseDelegate& TheDelegate = FOnUserInfoByUserIDResponseDelegate());

	
	TesserconAPI();
	~TesserconAPI();
	void Login(const FString& Username, const FString& Password);



private:
	FHttpModule* HttpModule;

	
	
};