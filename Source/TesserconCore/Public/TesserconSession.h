#pragma once
//Matrix
#include "MatrixCommon.h"
//TesserconCore
#include "TesserconAPI.h"
#include "TesserconCoreCommon.h"
//Vivox
#include "VivoxCore.h"
#include "VivoxCoreCommon.h"
//SQLite ORM
#include "Database.h"

class MatrixClient;
class MatrixSession;

struct FMatrixCallInviteWithRoomID
{
	FMatrixCallEvent CallEvent;
	FString RoomID;

	FMatrixCallInviteWithRoomID(const FMatrixCallEvent& InEvent, const FString& InRoomID) : CallEvent(InEvent), RoomID(InRoomID)
	{	}
	FMatrixCallInviteWithRoomID()
	{	}
};

class TESSERCONCORE_API TesserconSession : public FTickableGameObject,public DatabaseModel
{
	// TODO:: Could Change this 
	DECLARE_DELEGATE_OneParam(FOnVivoxAPIResponseDelegate, VivoxCoreError);
public:
	DECLARE_DELEGATE_OneParam(FOnTesserconSessionResponseDelegate, ETesserconError);
	
	DECLARE_EVENT_OneParam(TesserconSession,FOnTesserconLoginResponse,ETesserconError);
	FOnTesserconLoginResponse d_EventLoginStatusResponse;
	

	DECLARE_EVENT_OneParam(TesserconSession, FLoginStateDelegate, ETesserconLoginState);
	FLoginStateDelegate d_EventOnLoginStateChanged;

	DECLARE_EVENT_OneParam(TesserconClient, FOnCallStateChanged, FTesserconCallState);
	FOnCallStateChanged d_EventOnCallStateChanged;

	DECLARE_EVENT_ThreeParams(TesserconClient, FOnMatrixCallEventProcessed, EMatrixCallEventType, FString, FMatrixCallEvent);
	FOnMatrixCallEventProcessed d_EventOnMatrixCallEventProcessed;

	DECLARE_EVENT_OneParam(TesserconClient, FOnAudioDevicesAvailabiltyChanged, bool);
	FOnAudioDevicesAvailabiltyChanged d_EventOnMicAvailablityChanged;
	FOnAudioDevicesAvailabiltyChanged d_EventOnSpeakerAvailablityChanged;


	DECLARE_EVENT_OneParam(TesserconSession, FOnTesserconJoinVivoxResponse, ETesserconError);
	FOnTesserconJoinVivoxResponse d_EvenJoinVivoxNearByStatusResponse;
	

	TesserconSession();
	~TesserconSession();
	void Initialize();

	ETesserconLoginState GetLoginState() const;

	//***** TesserconCore Module *****//
	void Login(const FString& Username, const FString& Password);
	void Logout();
	void ForgotPassword(const FString& EmailID, const TesserconAPI::FOnForgotPasswordForUserResponseDelegate& TheDelegate);
	
	FTesserconUserInfo GetLoggedInUserInfo();
	void GetUserInfoByUsername(const FString& InUsername, const TesserconAPI::FOnUserInfoByUserIDResponseDelegate& TheDelegate = TesserconAPI::FOnUserInfoByUserIDResponseDelegate()) const;

	TArray<FTesserconEventsList> GetEventList();
	void GetEventInfoByEventID(const FString& EventID, const TesserconAPI::FOnEventInfoForUserResponseDelegate& TheDelegate = TesserconAPI::FOnEventInfoForUserResponseDelegate());
	void GetEventOrganiserByEventID(const FString& EventID, const TesserconAPI::FOnEventOrganiserResponseDelegate& TheDelegate = TesserconAPI::FOnEventOrganiserResponseDelegate());
	void GetEventSponsorsByEventID(const FString& EventID, const TesserconAPI::FOnEventSponsorsResponseDelegate& TheDelegate = TesserconAPI::FOnEventSponsorsResponseDelegate());

	void GetBoothListForEvent(const FString& EventID, TArray<FTesserconBoothList>& BoothList);
	void GetBoothInfoByBoothID(const FString& BoothID, const TesserconAPI::FOnBoothInfoForEventResponseDelegate& TheDelegate = TesserconAPI::FOnBoothInfoForEventResponseDelegate());
	
	//***** Vivox Plugin *****//
	const IAudioDevice* GetActiveMic() const;

	const IAudioDevice* GetActiveSpeaker() const;

	void SetActiveMic(const FString& MicName);

	void SetActiveSpeaker(const FString& SpeakerName);

	IAudioDevices* GetInputAudioDevices();

	IAudioDevices* GetOutputAudioDevices();

	int GetSpeakerVolume();

	void SetSpeakerVolume(float InVolume);

	bool IsMicAvailable() const
	{
		return bIsMicAvailable;
	}

	bool IsSpeakerAvailable() const
	{
		return bIsSpeakerAvailable;
	}

	void SetSpeakerMuted(bool bIsEnabled);
	bool GetSpeakerMuted();
	void SetMicMuted(bool bIsEnabled);
	bool GetMicMuted();

	//***** MatrixCore Module *****//
	void EventLogin();

	//Matrix call handlers
	FTesserconCallState GetCallState();
	void PlaceCall(const FString& RoomID);
	void AnswerCall(const FString& RoomID, const FMatrixCallEvent& CallInvite);
	void EndCall();
	void HangupCallInvite(const FString& RoomID, const FString& CallID, const FOnTesserconSessionResponseDelegate& TheDelegate = FOnTesserconSessionResponseDelegate());

	//Matrix call event handlers
	void SendMatrixCallInvite(const FString& RoomID);
	void SendMatrixCallAnswer(const FString& RoomID, const FString& CallID);
	void SendMatrixCallHangup(const FString& RoomID, const FString& CallID, const FOnTesserconSessionResponseDelegate& TheDelegate = FOnTesserconSessionResponseDelegate());

	// Calling Initialization events
	void BindMatrixCallingEvents(bool bDoBind, MatrixSession& MSession);

	//
	// TODO: check if they need to public
	VivoxCoreError JoinVivoxChannel(const FString& ChannelName, EVivoxChannelKey ChannelKey, const FOnVivoxAPIResponseDelegate& TheDelegate = FOnVivoxAPIResponseDelegate());

	void LeaveVivoxChannels(EVivoxChannelKey ChannelKey);

	void Update3DPosition(APawn* Pawn);




	//
	void UnSetNearByChannelTransmissionMode();
    void SetNearByChannelTransmissionMode();

private:
	//***** TesserconCore Module *****//
	TSharedPtr<TesserconAPI> m_NativeAPI;
	
	TArray<FTesserconEventsList> m_EventList;

	FTesserconCallState m_CallState;

	EEventSubscriptionStatus m_EventSubscriptionStatus = EEventSubscriptionStatus::Unknown;

	/** Begin FTickableGameObject overrides */
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const;
	virtual bool IsTickableInEditor() const { return true; }
	/** End FTickableGameObject overrides */

	void OnLoginResponseReceived(ETesserconError Error, const FTesserconLoginResponse& Response, const FString& ErrorMessage);
	
	void BroadcastLoginState();

	void UpdateLoginState(ETesserconLoginState State);

	FString m_LoggedInUserEmail;

	FString m_LoggedInUserPassword;

	FString m_MatrixUsername;

	FString m_ServerPrimaryKey;

	FTesserconTokens m_Tokens;

	FTesserconUserInfo m_LoggedInUserInfo;

	ETesserconLoginState m_TessLoginState = ETesserconLoginState::LoggedOut;

	//***** Database ORM Module *****//
	Database* p_DB;
	
	void SetupDatabase();
	
	void ExportMembers() override;
	
	void ImportMembers(char** Vals) override;

	//***** Matrix Module *****//
	MatrixClient* p_ChatClient = nullptr;

	bool bMatrixSessionHandlersBound = false;

	TMap<FString, FMatrixCallInviteWithRoomID> m_ValidCallInvitesMap;

	//Matrix Bind SessionHandler
	void BindMatrixSessionHandlers(bool DoBind, MatrixSession& Session);

	void OnTeamRemoved(FString RoomID);

	void OnFriendRemoved(FString RoomID);


	void OnMatrixLoginStateChanged(ELoginState State);

	void OnMatrixCallEventReceived(EMatrixCallEventType CallEventType, FString InRoomID, FMatrixCallEvent CallEvent);

	void PlaceCallBasic(const FString& RoomID);

	FMatrixCallInviteWithRoomID GetValidCallInviteForRoom(const FString& RoomID);
	
	//***** Vivox Plugin *****//
	DECLARE_DELEGATE_OneParam(FOnVivoxAPIResponseDelegate, VivoxCoreError);

	bool bVivoxInitialized = false;

	ChannelId m_VivoxGlobalChannel;

	ChannelId m_VivoxVoiceCallChannel;

	ChannelId m_VivoxNearByChannel;

	ChannelId ConnectedPositionalChannel;

	AccountId m_VivoxLoggedInAccountID;

	IClient* p_VivoxVoiceClient;

	LoginState m_VivoxLoginState = LoginState::LoggedOut;

	bool bIsMicAvailable = false;

	bool bIsSpeakerAvailable = false;

	CachedProperty<FVector> CachedPosition = CachedProperty<FVector>(FVector());
	CachedProperty<FVector> CachedForwardVector = CachedProperty<FVector>(FVector());
	CachedProperty<FVector> CachedUpVector = CachedProperty<FVector>(FVector());

	VivoxCoreError InitializeVivox(int logLevel);

	VivoxCoreError LoginToVivox();

	void LogoutOfVivox();

	void OnVivoxLoginStateChanged(LoginState State);

	void BindVivoxLoginSessionHandlers(bool DoBind, ILoginSession& LoginSession);

	void BindVivoxChannelSessionHandlers(bool DoBind, IChannelSession& ChannelSession);

	void OnVivoxChannelParticipantAdded(const IParticipant& Participant);
	void OnVivoxChannelParticipantRemoved(const IParticipant& Participant);
	void OnVivoxChannelParticipantUpdated(const IParticipant& Participant);
	void OnVivoxChannelAudioStateChanged(const IChannelConnectionState& State);
	void OnVivoxChannelTextStateChanged(const IChannelConnectionState& State);
	void OnVivoxChannelTextMessageReceived(const IChannelTextMessage& Message);

	//3D positional chat
	bool Get3DValuesAreDirty() const;
	void Clear3DValuesAreDirty();

	VivoxCoreError SetVivoxTransmissionMode(EVivoxChannelKey ChannelKey);

	bool AreAudioDevicesLoaded();

	void BindAudioDevicesEvents(bool bDoBind);

	void PlaceCallVivox(const FString& RoomID, const FString& CallID, ECallType CallType);
	void EndCall(bool bIsCallSwitching, bool bSendCallHangup = true, const FOnTesserconSessionResponseDelegate& TheDelegate = FOnTesserconSessionResponseDelegate());
	void EndCallOnHangupReceived();
	void LeaveVivoxVoiceCallChannel();

	void RemoveEventFromValidCallInvites(const FString& CallID);

	

	void VivoxOnEffectiveInputDeviceChanged(const IAudioDevice& AudioDevice);

	void VivoxOnEffectiveOutputDeviceChanged(const IAudioDevice& AudioDevice);

	void UpdateCallState(const FTesserconCallState& NewCallState);

public:

	
	//NearByChat

	// TODO: remove this after demo
	VivoxCoreError SendTextMessage(const FString& ChannelName, const FString& Message, EVivoxChannelKey ChannelKey, const FOnVivoxAPIResponseDelegate& TheDelegate = FOnVivoxAPIResponseDelegate());

	// TODO: is it needed ?
	TSharedPtr<IChannelSession> GetChannelSessionFromName(const FString& ChannelName);

	IChannelSession& GetChannelSessionFromKey(EVivoxChannelKey Key);


	
};

