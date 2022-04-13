//Tessercon
#include "TesserconSession.h"
#include "VivoxToken.h"
//Matrix
#include "MatrixCore.h"
//SQLite
#include "Database.h"

#include "Engine/World.h"


#define VIVOX_VOICE_SERVER TEXT("https://mt1s.www.vivox.com/api2")
#define VIVOX_VOICE_DOMAIN TEXT("mt1s.vivox.com")
#define VIVOX_VOICE_ISSUER TEXT("tesser3512-te70-dev")


//Its used for getClassName Macro 
EXPORT_CLASS(TesserconSession)

TesserconSession::TesserconSession()
{
	UE_LOG(LogTemp, Warning, TEXT(" In TesserconSession::TesserconSession() -------------------"));
}

TesserconSession::~TesserconSession()
{
	UE_LOG(LogTemp, Warning, TEXT(" In TesserconSession::~TesserconSession() -------------------"));
}

void TesserconSession::Initialize()
{
	m_NativeAPI = TSharedPtr<TesserconAPI>(new TesserconAPI());

	p_VivoxVoiceClient = &static_cast<FVivoxCoreModule*>(&FModuleManager::Get().LoadModuleChecked(TEXT("VivoxCore")))->VoiceClient();

	if (m_NativeAPI == nullptr) return;
		
	m_NativeAPI->d_OnLoginResponseReceived.BindRaw(this, &TesserconSession::OnLoginResponseReceived);

	BindAudioDevicesEvents(true);

	InitializeVivox(1);

	m_CallState = FTesserconCallState(ETesserconCallStatus::Available);
	
	p_DB = Database::GetDatabaseInstance();

	if (p_DB != nullptr)
	{
		SetupDatabase();
	}
}

//Its Part of ORM
void TesserconSession::ExportMembers()
{
	REGISTER_MEMBER(m_LoggedInUserEmail, ESQLiteDataType::TEXT, ESQLiteConstraint::UNIQUE);

	REGISTER_MEMBER(m_LoggedInUserPassword, ESQLiteDataType::TEXT, ESQLiteConstraint::NONE);

	REGISTER_MEMBER(m_ServerPrimaryKey, ESQLiteDataType::TEXT, ESQLiteConstraint::NONE);

	REGISTER_MEMBER(m_Tokens.AccessToken, ESQLiteDataType::TEXT, ESQLiteConstraint::NONE);

	REGISTER_MEMBER(m_Tokens.ExpiresIn, ESQLiteDataType::TEXT, ESQLiteConstraint::NONE);

	REGISTER_MEMBER(m_Tokens.RefreshToken, ESQLiteDataType::TEXT, ESQLiteConstraint::NONE);

	REGISTER_MEMBER(m_MatrixUsername, ESQLiteDataType::TEXT, ESQLiteConstraint::NONE);
}

void TesserconSession::ImportMembers(char** Vals)
{
	m_LoggedInUserEmail = FString(UTF8_TO_TCHAR(Vals[0]));

	m_LoggedInUserPassword = FString(UTF8_TO_TCHAR(Vals[1]));

	m_ServerPrimaryKey = FString(UTF8_TO_TCHAR(Vals[2]));

	m_Tokens.AccessToken = FString(UTF8_TO_TCHAR(Vals[3]));

	FDateTime::Parse(FString(UTF8_TO_TCHAR(Vals[4])), m_Tokens.ExpiresIn);

	m_Tokens.RefreshToken = FString(UTF8_TO_TCHAR(Vals[5]));

	m_MatrixUsername = FString(UTF8_TO_TCHAR(Vals[6]));
}

void TesserconSession::SetupDatabase()
{
	if (p_DB != nullptr)
	{
		p_DB->CreateTable<TesserconSession>();
	}

	TesserconSession* DBSession = p_DB->Read<TesserconSession>("");
	if (DBSession->m_LoggedInUserEmail.IsEmpty() || 
		DBSession->m_LoggedInUserPassword.IsEmpty() || 
		DBSession->m_ServerPrimaryKey.IsEmpty()|| 
		DBSession->m_Tokens.AccessToken.IsEmpty() || 
		DBSession->m_MatrixUsername.IsEmpty()) return;

	m_Tokens.AccessToken = DBSession->m_Tokens.AccessToken;
	m_LoggedInUserEmail = DBSession->m_LoggedInUserEmail;
	m_LoggedInUserPassword = DBSession->m_LoggedInUserPassword;
	m_ServerPrimaryKey = DBSession->m_ServerPrimaryKey;
	m_MatrixUsername =DBSession->m_MatrixUsername;
	UpdateLoginState(ETesserconLoginState::LogginIn);
}

void TesserconSession::Login(const FString& Username, const FString& Password)
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->Login(Username, Password);
	}
}

void TesserconSession::OnLoginResponseReceived(ETesserconError Error, const FTesserconLoginResponse& Response, const FString& ErrorMessage)
{
	if (Error == ETesserconError::Success)
	{
		m_LoggedInUserEmail = Response.UserEmail;
		m_LoggedInUserPassword = Response.Password;
		m_Tokens = Response.Tokens;

		//GetLoggedInUserInfo
		if (m_NativeAPI != nullptr)
		{
			TesserconAPI::FOnLoggedinUserInfoResponseDelegate UserInfoCallBack;
			UserInfoCallBack.BindLambda([this](ETesserconError Error, const FTesserconUserInfo& ResponseUserInfo) {
					if(Error == ETesserconError::Success)
					{
						m_LoggedInUserInfo = ResponseUserInfo;
						m_MatrixUsername = m_LoggedInUserInfo.Username;
						LoginToVivox();
					}
				});
			m_NativeAPI->GetLoggedinUserInfo(m_Tokens.AccessToken, UserInfoCallBack);
		}
		//p_ChatClient = static_cast<FMatrixCoreModule*>(&FModuleManager::Get().LoadModuleChecked(TEXT("MatrixCore")))->GetMatrixClient();
		p_VivoxVoiceClient = &static_cast<FVivoxCoreModule*>(&FModuleManager::Get().LoadModuleChecked(TEXT("VivoxCore")))->VoiceClient();

		UpdateLoginState(ETesserconLoginState::LoggedIn);

		if (p_DB != nullptr)
		{
			p_DB->ClearTable<TesserconSession>();

			p_DB->Write(this);
		}		
	}
	d_EventLoginStatusResponse.Broadcast(Error);
}

void TesserconSession::UpdateLoginState(ETesserconLoginState State)
{
	m_TessLoginState = State;
	
	if (State == ETesserconLoginState::LoggedIn && m_NativeAPI != nullptr)
	{
		TesserconAPI::FOnSubscribedEventsListForUserResponseDelegate SubscribedEventsCallback;
		SubscribedEventsCallback.BindLambda([this](ETesserconError ErrorStatus, TArray<FTesserconEventsList>EventList)
			{
				if (ErrorStatus == ETesserconError::Success)
				{
					m_EventList = EventList;
					if (m_EventList.Num())
					{
						m_EventSubscriptionStatus = EEventSubscriptionStatus::Active;
						//BroadcastLoginState();
					}
					else
					{
						m_EventSubscriptionStatus = EEventSubscriptionStatus::None;
						//BroadcastLoginState();
					}
				
				}
				else
				{
					m_EventSubscriptionStatus = EEventSubscriptionStatus::None;
					//BroadcastLoginState();
				}
			});
		m_NativeAPI->GetEventsSubscribedListForUser(m_Tokens.AccessToken, SubscribedEventsCallback);
	}
	else if (State == ETesserconLoginState::LoggedOut)
	{
		m_LoggedInUserEmail = "";
		m_LoggedInUserPassword = "";
		m_ServerPrimaryKey = "";
		m_MatrixUsername = "";
		m_EventList.Empty();
		m_EventSubscriptionStatus = EEventSubscriptionStatus::Unknown;
		BroadcastLoginState();
	}
}

TArray<FTesserconEventsList> TesserconSession::GetEventList()
{
	return m_EventList;
}

void TesserconSession::GetBoothListForEvent(const FString& EventID, TArray<FTesserconBoothList>& OutBoothList)
{
	if (m_NativeAPI != nullptr)
	{
		TesserconAPI::FOnBoothListForEventResponseDelegate BoothListCallback;
		BoothListCallback.BindLambda([&](ETesserconError ErrorStatus, TArray<FTesserconBoothList>InBoothList)
			{
				if (ErrorStatus == ETesserconError::Success)
				{
					OutBoothList = InBoothList;
				}			
			});

		m_NativeAPI->GetBoothListForEvent(m_Tokens.AccessToken, EventID, BoothListCallback);
	}
}

void TesserconSession::GetEventInfoByEventID(const FString& EventID, const TesserconAPI::FOnEventInfoForUserResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->GetEventInfoForUser(m_Tokens.AccessToken, EventID, TheDelegate);
	}
}

void TesserconSession::GetEventOrganiserByEventID(const FString& EventID, const TesserconAPI::FOnEventOrganiserResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->GetOrganiserForEvent(m_Tokens.AccessToken, EventID, TheDelegate);
	}
}

void TesserconSession::GetEventSponsorsByEventID(const FString& EventID, const TesserconAPI::FOnEventSponsorsResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->GetSponsorsForEvent(m_Tokens.AccessToken, EventID, TheDelegate);
	}
}

void TesserconSession::GetBoothInfoByBoothID(const FString& BoothID, const TesserconAPI::FOnBoothInfoForEventResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->GetBoothInfoForEvent(m_Tokens.AccessToken, BoothID, TheDelegate);
	}
}

void TesserconSession::ForgotPassword(const FString& EmailID, const TesserconAPI::FOnForgotPasswordForUserResponseDelegate& TheDelegate)
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->ForgotPassword(EmailID, TheDelegate);
	}
}

void TesserconSession::BroadcastLoginState()
{
	if (m_TessLoginState == ETesserconLoginState::LoggedIn)
	{
		if (m_EventSubscriptionStatus == EEventSubscriptionStatus::Active)
		{
			d_EventOnLoginStateChanged.Broadcast(ETesserconLoginState::LoggedIn);
		}
		else if (m_EventSubscriptionStatus == EEventSubscriptionStatus::None)
		{
			d_EventOnLoginStateChanged.Broadcast(ETesserconLoginState::LoggedInNoActiveEvents);
		}
	}
	else if (m_TessLoginState == ETesserconLoginState::LoggedOut)
	{
		d_EventOnLoginStateChanged.Broadcast(ETesserconLoginState::LoggedOut);
	}
}

void TesserconSession::OnMatrixCallEventReceived(EMatrixCallEventType CallEventType, FString InRoomID, FMatrixCallEvent CallEvent)
{
	// process the call request here
	UE_LOG(LogTemp, Warning, TEXT("TesserconClient::OnMatrixCallEventReceived  event type : %s RoomID :  %s, CallID: %s, Sender: %s "), *ConvertMatrixCallEventTypeToString(CallEventType), *InRoomID, *CallEvent.CallID, *CallEvent.Sender);

	if (p_ChatClient != nullptr)
	{
		FString MatrixLoggedInUserID = p_ChatClient->GetSession().GetLoggedInUserID();

		if (CallEventType == EMatrixCallEventType::Invite)
		{
			bool bShouldBrodcastEvent = true;
			// glare condition 2 when there is an existing call and new invite is also send, then accept the old one & hangup the new one
			const FMatrixCallInviteWithRoomID ExistingValidInvite = GetValidCallInviteForRoom(InRoomID);
			if (ExistingValidInvite.CallEvent.CallID != "")
			{
				bShouldBrodcastEvent = false;

				if (ExistingValidInvite.CallEvent.ExpireTime < CallEvent.ExpireTime)
				{
					if (MatrixLoggedInUserID != ExistingValidInvite.CallEvent.Sender)
					{
						HangupCallInvite(InRoomID, CallEvent.CallID);
						AnswerCall(ExistingValidInvite.RoomID, ExistingValidInvite.CallEvent);
					}
				}
			}

			if (bShouldBrodcastEvent && MatrixLoggedInUserID != CallEvent.Sender)
			{
				// use this for checking timer 
				d_EventOnMatrixCallEventProcessed.Broadcast(CallEventType, InRoomID, CallEvent);
			}
			m_ValidCallInvitesMap.Add(CallEvent.CallID, FMatrixCallInviteWithRoomID(CallEvent, InRoomID));
		}
		else if (CallEventType == EMatrixCallEventType::Answer)
		{
			RemoveEventFromValidCallInvites(CallEvent.CallID);

			if (m_CallState.CallID == CallEvent.CallID && m_CallState.RoomID == InRoomID && m_CallState.CallType == ECallType::Individual && MatrixLoggedInUserID != CallEvent.Sender)
			{
				// TODO: change this to connecting??
				UpdateCallState(FTesserconCallState(m_CallState.RoomID, m_CallState.CallID, m_CallState.CallType, ETesserconCallStatus::Connecting));
				PlaceCallVivox(InRoomID, m_CallState.CallID, m_CallState.CallType);
			}
		}
		else if (CallEventType == EMatrixCallEventType::Hangup)
		{
			RemoveEventFromValidCallInvites(CallEvent.CallID);

			if (m_CallState.CallID == CallEvent.CallID && m_CallState.CallType == ECallType::Individual) //&& MatrixLoggedInUserID != CallEvent.Sender)
			{
				// End call
				EndCallOnHangupReceived();
			}
			else
			{
				d_EventOnMatrixCallEventProcessed.Broadcast(CallEventType, InRoomID, CallEvent);
			}
		}
	}
}

VivoxCoreError TesserconSession::InitializeVivox(int logLevel)
{
	if (bVivoxInitialized)
	{
		return VxErrorSuccess;
	}

	VivoxConfig Config;
	Config.SetLogLevel((vx_log_level)logLevel);

	if (p_VivoxVoiceClient == nullptr) return VX_E_FAILED;

	VivoxCoreError Status = p_VivoxVoiceClient->Initialize(Config);
	if (Status != VxErrorSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("Initialize failed: %s (%d)"), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)), Status);
	}
	else
	{
		bVivoxInitialized = true;
	}

	return Status;
}

VivoxCoreError TesserconSession::LoginToVivox()
{
	if (p_VivoxVoiceClient == nullptr) return VxErrorNotInitialized;

	if (m_VivoxLoginState == LoginState::LoggedIn)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Already logged in"));
		return VxErrorSuccess;
	}

	if (m_VivoxLoginState == LoginState::LoggingIn)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Already logging in"));
		return VxErrorSuccess;
	}

	if (!bVivoxInitialized)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Not initialized"));
		return VxErrorNotInitialized;
	}

	m_VivoxLoggedInAccountID = AccountId(VIVOX_VOICE_ISSUER, m_MatrixUsername, VIVOX_VOICE_DOMAIN);

	if (m_VivoxLoggedInAccountID.IsEmpty()) return VxErrorNotInitialized;

	ILoginSession& LoginSession = p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID);

	// IMPORTANT: in production, developers should NOT use the insecure client-side token generation methods.
	// To generate secure access tokens, call GenerateClientLoginToken or a custom implementation from your game server.
	// This is important not only to secure access to chat features, but tokens issued by the client could
	// appear expired if the client's clock is set incorrectly, resulting in rejection.
	FString LoginToken;
	FVivoxToken::GenerateClientLoginToken(LoginSession, LoginToken);

	UE_LOG(LogTemp, Verbose, TEXT("Logging in %s with token %s"), *m_MatrixUsername, *LoginToken);

	ILoginSession::FOnBeginLoginCompletedDelegate OnBeginLoginCompleteCallback;
	OnBeginLoginCompleteCallback.BindLambda([this, &LoginSession](VivoxCoreError Status)
		{
			if (VxErrorSuccess != Status)
			{
				UE_LOG(LogTemp, Error, TEXT("Login failure for %s: %s (%d)"), *m_MatrixUsername, ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)), Status);
				BindVivoxLoginSessionHandlers(false, LoginSession);
				m_VivoxLoggedInAccountID = AccountId();
				OnVivoxLoginStateChanged(LoginState::LoggedOut);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Login success for %s"), *m_MatrixUsername);
				OnVivoxLoginStateChanged(LoginState::LoggedIn);
				BindVivoxLoginSessionHandlers(true, LoginSession);
			}
		});

	OnVivoxLoginStateChanged(LoginState::LoggingIn);

	return LoginSession.BeginLogin(VIVOX_VOICE_SERVER, LoginToken, OnBeginLoginCompleteCallback);
}

void TesserconSession::LogoutOfVivox()
{
	if (p_VivoxVoiceClient == nullptr) return;

	if (m_VivoxLoginState == LoginState::LoggingOut || m_VivoxLoginState == LoginState::LoggedOut)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Not logged in, skipping logout"));
		return;
	}

	ILoginSession& LoginSession = p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID);

	LoginSession.Logout();

	m_VivoxLoggedInAccountID = AccountId();

	OnVivoxLoginStateChanged(LoginState::LoggedOut);
}

void TesserconSession::OnVivoxLoginStateChanged(LoginState State)
{
	m_VivoxLoginState = State;

	if (State == LoginState::LoggedOut)
	{
		m_VivoxLoggedInAccountID = AccountId();
	}

	if (State == LoginState::LoggedIn)
	{
		// TODO: check if the login is 
		bool bAreAudioDevicesLoaded = AreAudioDevicesLoaded();

		if (bAreAudioDevicesLoaded)
		{
			UE_LOG(LogTemp, Warning, TEXT("TesserconClient::OnVivoxLoginStateChanged audio devices are loaded"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("TesserconClient::OnVivoxLoginStateChanged no audio devices are detected"));
		}
	}

	BroadcastLoginState();
}

void TesserconSession::BindVivoxLoginSessionHandlers(bool DoBind, ILoginSession& LoginSession)
{
	if (DoBind)
	{
		LoginSession.EventStateChanged.AddRaw(this, &TesserconSession::OnVivoxLoginStateChanged);
	}
	else
	{
		LoginSession.EventStateChanged.RemoveAll(this);
	}
}

void TesserconSession::BindVivoxChannelSessionHandlers(bool DoBind, IChannelSession& ChannelSession)
{
	if (DoBind)
	{
		ChannelSession.EventAfterParticipantAdded.AddRaw(this, &TesserconSession::OnVivoxChannelParticipantAdded);
		ChannelSession.EventBeforeParticipantRemoved.AddRaw(this, &TesserconSession::OnVivoxChannelParticipantRemoved);
		ChannelSession.EventAfterParticipantUpdated.AddRaw(this, &TesserconSession::OnVivoxChannelParticipantUpdated);
		ChannelSession.EventAudioStateChanged.AddRaw(this, &TesserconSession::OnVivoxChannelAudioStateChanged);
		ChannelSession.EventTextStateChanged.AddRaw(this, &TesserconSession::OnVivoxChannelTextStateChanged);
		ChannelSession.EventTextMessageReceived.AddRaw(this, &TesserconSession::OnVivoxChannelTextMessageReceived);
	}
	else
	{
		ChannelSession.EventAfterParticipantAdded.RemoveAll(this);
		ChannelSession.EventBeforeParticipantRemoved.RemoveAll(this);
		ChannelSession.EventAfterParticipantUpdated.RemoveAll(this);
		ChannelSession.EventAudioStateChanged.RemoveAll(this);
		ChannelSession.EventTextStateChanged.RemoveAll(this);
		ChannelSession.EventTextMessageReceived.RemoveAll(this);
	}
}

void TesserconSession::OnVivoxChannelParticipantAdded(const IParticipant& Participant)
{
	ChannelId Channel = Participant.ParentChannelSession().Channel();
	UE_LOG(LogTemp, Log, TEXT("User %s has joined channel %s (self = %s)"), *Participant.Account().Name(), *Channel.Name(), Participant.IsSelf() ? TEXT("true") : TEXT("false"));
}

void TesserconSession::OnVivoxChannelParticipantRemoved(const IParticipant& Participant)
{
	ChannelId Channel = Participant.ParentChannelSession().Channel();
	UE_LOG(LogTemp, Log, TEXT("User %s has left channel %s (self = %s)"), *Participant.Account().Name(), *Channel.Name(), Participant.IsSelf() ? TEXT("true") : TEXT("false"));
}

void TesserconSession::OnVivoxChannelParticipantUpdated(const IParticipant& Participant)
{
	if (Participant.IsSelf()) 
	{
		UE_LOG(LogTemp, Log, TEXT("Self Participant Updated (audio=%d, text=%d, speaking=%d)"), Participant.InAudio(), Participant.InText(), Participant.SpeechDetected());
	}
}

void TesserconSession::OnVivoxChannelAudioStateChanged(const IChannelConnectionState& State)
{
	UE_LOG(LogTemp, Log, TEXT("ChannelSession Audio State Change in %s: %s"), *State.ChannelSession().Channel().Name(), *UEnumShortToString(ConnectionState, State.State()));
}

void TesserconSession::OnVivoxChannelTextStateChanged(const IChannelConnectionState& State)
{
	UE_LOG(LogTemp, Log, TEXT("ChannelSession Text State Change in %s: %s"), *State.ChannelSession().Channel().Name(), *UEnumShortToString(ConnectionState, State.State()));
}

void TesserconSession::OnVivoxChannelTextMessageReceived(const IChannelTextMessage& Message)
{
	UE_LOG(LogTemp, Log, TEXT("Message Received from %s: %s"), *Message.Sender().Name(), *Message.Message());
	UE_LOG(LogTemp, Warning, TEXT("time stamp at message received %s"), *FDateTime::Now().ToString());
}

bool TesserconSession::Get3DValuesAreDirty() const
{
	return (CachedPosition.IsDirty() || CachedForwardVector.IsDirty() || CachedUpVector.IsDirty());
}

void TesserconSession::Clear3DValuesAreDirty()
{
	CachedPosition.SetDirty(false);
	CachedForwardVector.SetDirty(false);
	CachedUpVector.SetDirty(false);
}

VivoxCoreError TesserconSession::SetVivoxTransmissionMode(EVivoxChannelKey ChannelKey)
{
	if (p_VivoxVoiceClient == nullptr) return VxErrorNotInitialized;

	if (ChannelKey == EVivoxChannelKey::NearByChannel)
	{
		return p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID).SetTransmissionMode(TransmissionMode::Single, m_VivoxNearByChannel);
	}
	else if (ChannelKey == EVivoxChannelKey::VoiceCallChannel)
	{
		return p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID).SetTransmissionMode(TransmissionMode::Single, m_VivoxVoiceCallChannel);
	}
	else
	{
		return p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID).SetTransmissionMode(TransmissionMode::None);
	}
}

bool TesserconSession::AreAudioDevicesLoaded()
{
	if (p_VivoxVoiceClient == nullptr) return false;

	IAudioDevices& InputDevices = p_VivoxVoiceClient->AudioInputDevices();

	IAudioDevices& OutputDevices = p_VivoxVoiceClient->AudioOutputDevices();

	const IAudioDevice& EffectiveInputDevice = InputDevices.EffectiveDevice();
	const IAudioDevice& EffectiveOutputDevice = OutputDevices.EffectiveDevice();

	return EffectiveInputDevice.Id() != "No Device" && EffectiveOutputDevice.Id() != "No Device";
}

void TesserconSession::BindAudioDevicesEvents(bool bDoBind)
{
	if (p_VivoxVoiceClient == nullptr) return;

	IAudioDevices& InputDevices = p_VivoxVoiceClient->AudioInputDevices();
	IAudioDevices& OutputDevices = p_VivoxVoiceClient->AudioOutputDevices();

	if (bDoBind)
	{
		InputDevices.EventEffectiveDeviceChanged.AddRaw(this, &TesserconSession::VivoxOnEffectiveInputDeviceChanged);
		OutputDevices.EventEffectiveDeviceChanged.AddRaw(this, &TesserconSession::VivoxOnEffectiveOutputDeviceChanged);
	}
	else
	{
		InputDevices.EventEffectiveDeviceChanged.RemoveAll(this);
		OutputDevices.EventEffectiveDeviceChanged.RemoveAll(this);
	}
}

void TesserconSession::PlaceCallVivox(const FString& RoomID, const FString& CallID, ECallType CallType)
{
	FOnVivoxAPIResponseDelegate OnJoinSuccessCallback;
	OnJoinSuccessCallback.BindLambda([this, RoomID, CallID, CallType](VivoxCoreError ErrorStatus)
		{
			if (ErrorStatus == VxErrorSuccess)
				UpdateCallState(FTesserconCallState(RoomID, CallID, CallType, ETesserconCallStatus::OnCall));

			else
				UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
		});
	FString FromString = ":";
	FString ToString = "-";
	FString ValidRoomID = RoomID.Replace(*FromString, *ToString);
	JoinVivoxChannel(ValidRoomID, EVivoxChannelKey::VoiceCallChannel, OnJoinSuccessCallback);
}

void TesserconSession::EndCall(bool bIsCallSwitching, bool bSendCallHangup, const FOnTesserconSessionResponseDelegate& TheDelegate)
{
	if (m_CallState.CallStatus == ETesserconCallStatus::OnCall || m_CallState.CallStatus == ETesserconCallStatus::Dialing)
	{
		ETesserconCallStatus PreviousCallStatus = m_CallState.CallStatus;
		// if team  call then end it
		UpdateCallState(FTesserconCallState(m_CallState.RoomID, m_CallState.CallID, m_CallState.CallType,
			ETesserconCallStatus::Processing));

		if (PreviousCallStatus == ETesserconCallStatus::OnCall)
		{
			// In dialing vivox channels are not connected
			LeaveVivoxVoiceCallChannel();
		}

		if (m_CallState.CallType == ECallType::Team)
		{
			// no dialing for team
			TheDelegate.ExecuteIfBound(ETesserconError::Success);
			if (!bIsCallSwitching)
			{
				UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
			}
		}
		else if (m_CallState.CallType == ECallType::Individual)
		{
			FOnTesserconSessionResponseDelegate OnSendHangupCallback;
			OnSendHangupCallback.BindLambda([this, TheDelegate, bIsCallSwitching](ETesserconError ErrorStatus)
				{
					if (ErrorStatus == ETesserconError::Success)
					{
						if (!bIsCallSwitching)
						{
							UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
						}
					}
					else
					{
						UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
					}
					TheDelegate.ExecuteIfBound(ErrorStatus);
				});

			if (!bSendCallHangup)
			{
				// when the call hangup is received
				TheDelegate.ExecuteIfBound(ETesserconError::Success);
				UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
			}
			else
			{
				SendMatrixCallHangup(m_CallState.RoomID, m_CallState.CallID, OnSendHangupCallback);
			}
		}
		else
		{
			// TODO: unknown error
			TheDelegate.ExecuteIfBound(ETesserconError::Error);
		}
	}
	else
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Error);
	}
}

void TesserconSession::EndCallOnHangupReceived()
{
	EndCall(false, false);
}

void TesserconSession::LeaveVivoxVoiceCallChannel()
{
	LeaveVivoxChannels(EVivoxChannelKey::VoiceCallChannel);
}

void TesserconSession::RemoveEventFromValidCallInvites(const FString& CallID)
{
	if (m_ValidCallInvitesMap.Contains(CallID))
	{
		m_ValidCallInvitesMap.Remove(CallID);
	}
}

VivoxCoreError TesserconSession::JoinVivoxChannel(const FString& ChannelName, EVivoxChannelKey ChannelKey, const FOnVivoxAPIResponseDelegate& TheDelegate)
{
	if (p_VivoxVoiceClient == nullptr) return VxErrorNotInitialized;

	if (m_VivoxLoginState != LoginState::LoggedIn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not logged in; cannot join a channel"));
		return VxErrorNotLoggedIn;
	}

	if (ChannelKey == EVivoxChannelKey::NoChannel) return VxErrorSuccess;

	ensure(!m_MatrixUsername.IsEmpty());
	ensure(!ChannelName.IsEmpty());

	VivoxChannelProperties ChannelProperties = GetChannelPropertiesForChannelKey(ChannelKey);

	ILoginSession& LoginSession = p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID);
	// It's perfectly safe to add 3D properties to any channel type (they don't have any effect if the channel type is not Positional)
	ChannelId Channel = ChannelId(VIVOX_VOICE_ISSUER, ChannelName, VIVOX_VOICE_DOMAIN, ChannelProperties.Type, Channel3DProperties(500, 175, 1.0, EAudioFadeModel::InverseByDistance));
	IChannelSession& ChannelSession = LoginSession.GetChannelSession(Channel);

	// IMPORTANT: in production, developers should NOT use the insecure client-side token generation methods.
	// To generate secure access tokens, call GenerateClientJoinToken or a custom implementation from your game server.
	// This is important not only to secure access to Chat features, but tokens issued by the client could
	// appear expired if the client's clock is set incorrectly, resulting in rejection.
	FString JoinToken;
	FVivoxToken::GenerateClientJoinToken(ChannelSession, JoinToken);

	UE_LOG(LogTemp, Verbose, TEXT("Joining %s to %s with token %s"), *m_MatrixUsername, *ChannelName, *JoinToken);

	IChannelSession::FOnBeginConnectCompletedDelegate OnBeginConnectCompleteCallback;
	OnBeginConnectCompleteCallback.BindLambda([this, ChannelProperties, ChannelKey, &LoginSession, &ChannelSession, TheDelegate](VivoxCoreError Status)
		{
			if (VxErrorSuccess != Status)
			{
				UE_LOG(LogTemp, Error, TEXT("Join failure for %s: %s (%d)"), *ChannelSession.Channel().Name(), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)), Status);
				BindVivoxChannelSessionHandlers(false, ChannelSession); // Unbind handlers if we fail to join.
				LoginSession.DeleteChannelSession(ChannelSession.Channel()); // Disassociate this ChannelSession from the LoginSession.
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Join success for %s"), *ChannelSession.Channel().Name());

				//manage channels here 
				if (ChannelType::Positional == ChannelSession.Channel().Type())
				{
					ConnectedPositionalChannel = ChannelSession.Channel();
				}

				if (ChannelKey == EVivoxChannelKey::GlobalTextOnlyChannel)
				{
					m_VivoxGlobalChannel = ChannelSession.Channel();
				}

				else if (ChannelKey == EVivoxChannelKey::NearByChannel)
				{
					m_VivoxNearByChannel = ChannelSession.Channel();

					//TODO::
					d_EvenJoinVivoxNearByStatusResponse.Broadcast(ETesserconError::Success);
					
				}

				else if (ChannelKey == EVivoxChannelKey::VoiceCallChannel)
				{
					m_VivoxVoiceCallChannel = ChannelSession.Channel();
				}

				// TODO: check if this is needed

				if (ChannelProperties.bShouldTransmitOnJoin)
				{
					SetVivoxTransmissionMode(ChannelKey);
				}
			}

			TheDelegate.ExecuteIfBound(Status);
		});
	BindVivoxChannelSessionHandlers(true, ChannelSession);

	return ChannelSession.BeginConnect(ChannelProperties.bConnectAudio, ChannelProperties.bConnectText, ChannelProperties.bShouldTransmitOnJoin, JoinToken, OnBeginConnectCompleteCallback);

}

void TesserconSession::LeaveVivoxChannels(EVivoxChannelKey ChannelKey)
{
	if (m_VivoxLoginState != LoginState::LoggedIn || p_VivoxVoiceClient == nullptr)
	{
		return;
	}

	TArray<ChannelId> ChannelSessionKeys;
	p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID).ChannelSessions().GenerateKeyArray(ChannelSessionKeys);

	for (ChannelId SessionKey : ChannelSessionKeys)
	{
		bool bDoLeave = false;
		if (ChannelKey == EVivoxChannelKey::NearByChannel && m_VivoxNearByChannel.Name() == SessionKey.Name())
		{
			bDoLeave = true;
		}
		if (ChannelKey == EVivoxChannelKey::VoiceCallChannel && m_VivoxVoiceCallChannel.Name() == SessionKey.Name())
		{
			bDoLeave = true;
		}
		if (ChannelKey == EVivoxChannelKey::GlobalTextOnlyChannel && m_VivoxGlobalChannel.Name() == SessionKey.Name())
		{
			bDoLeave = true;
		}
		if (ChannelKey == EVivoxChannelKey::NoChannel)
		{
			bDoLeave = true;
		}

		if (bDoLeave)
		{
			UE_LOG(LogTemp, Log, TEXT("Disconnecting from channel %s"), *SessionKey.Name());
			BindVivoxChannelSessionHandlers(false, p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID).GetChannelSession(SessionKey));
			p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID).DeleteChannelSession(SessionKey);
		}
	}

	// clear the stored channels
	if (ChannelKey == EVivoxChannelKey::NearByChannel)
	{
		m_VivoxNearByChannel = ChannelId();
	}
	else if (ChannelKey == EVivoxChannelKey::VoiceCallChannel)
	{
		m_VivoxVoiceCallChannel = ChannelId();
	}
	else if (ChannelKey == EVivoxChannelKey::GlobalTextOnlyChannel)
	{
		m_VivoxGlobalChannel = ChannelId();
	}
	else // (ChannelKey == EVivoxChannelKey::NoChannel)
	{
		m_VivoxGlobalChannel = ChannelId();
		m_VivoxNearByChannel = ChannelId();
		m_VivoxNearByChannel = ChannelId();
	}
}


void TesserconSession::Update3DPosition(APawn* Pawn)
{
	/// Return if argument is invalid.
	if (NULL == Pawn)
		return;

	/// Return if we're not in a positional channel.
	/*if (ConnectedPositionalChannel.IsEmpty())

		return;*/


	if (ConnectedPositionalChannel.IsEmpty())
		return;


	/// Update cached 3D position and orientation.
	CachedPosition.SetValue(Pawn->GetActorLocation());
	CachedForwardVector.SetValue(Pawn->GetActorForwardVector());
	CachedUpVector.SetValue(Pawn->GetActorUpVector());

	/// Return If there's no change from cached values.
	if (!Get3DValuesAreDirty())
		return;

	/// Set new position and orientation in connected positional channel.
	//Tracer::MajorMethodPrologue("%s %s %s %s %s", *ConnectedPositionalChannel.Name(), *CachedPosition.GetValue().ToCompactString(), *CachedPosition.GetValue().ToCompactString(), *CachedForwardVector.GetValue().ToCompactString(), *CachedUpVector.GetValue().ToCompactString());
	//ILoginSession& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountID);

	ILoginSession& LoginSession = p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID);
	LoginSession.GetChannelSession(ConnectedPositionalChannel).Set3DPosition(CachedPosition.GetValue(), CachedPosition.GetValue(), CachedForwardVector.GetValue(), CachedUpVector.GetValue());


	//LoginSession.GetChannelSession(ConnectedPositionalChannel).Set3DPosition(CachedPosition.GetValue(), CachedPosition.GetValue(), CachedForwardVector.GetValue(), CachedUpVector.GetValue());



	Clear3DValuesAreDirty();
}


void TesserconSession::VivoxOnEffectiveInputDeviceChanged(const IAudioDevice& AudioDevice)
{
	if (AudioDevice.IsEmpty())
	{
		bIsMicAvailable = false;
	}
	else
	{
		bIsMicAvailable = true;
	}

	d_EventOnMicAvailablityChanged.Broadcast(bIsMicAvailable);
}

void TesserconSession::VivoxOnEffectiveOutputDeviceChanged(const IAudioDevice& AudioDevice)
{
	if (AudioDevice.IsEmpty())
		bIsSpeakerAvailable = false;

	else
		bIsSpeakerAvailable = true;

	d_EventOnSpeakerAvailablityChanged.Broadcast(bIsSpeakerAvailable);
}

void TesserconSession::UpdateCallState(const FTesserconCallState& NewCallState)
{
	m_CallState = NewCallState;
	d_EventOnCallStateChanged.Broadcast(NewCallState);
}

FTesserconCallState TesserconSession::GetCallState()
{
	return m_CallState;
}




void TesserconSession::Logout()
{
	UpdateLoginState(ETesserconLoginState::LoggingOut);
	if (p_DB != nullptr)
	{
		p_DB->ClearTable<TesserconSession>();
	}

	if (p_ChatClient != nullptr)
	{
		BindMatrixSessionHandlers(false, p_ChatClient->GetSession());
		p_ChatClient->Logout();
	}
	LogoutOfVivox();
	OnMatrixLoginStateChanged(ELoginState::LoggedOut);
	UpdateLoginState(ETesserconLoginState::LoggedOut);
}

ETesserconLoginState TesserconSession::GetLoginState() const
{
	return m_TessLoginState;
}

//LoggedInUserInfo
FTesserconUserInfo TesserconSession::GetLoggedInUserInfo()
{
	return m_LoggedInUserInfo;
}

//

TStatId TesserconSession::GetStatId() const
{
	return TStatId();
}

void TesserconSession::Tick(float DeltaTime)
{

	FString MatrixLoggedInUserID;
	if (p_ChatClient != nullptr)
	{
		MatrixLoggedInUserID = p_ChatClient->GetSession().GetLoggedInUserID();

		TArray<FMatrixCallInviteWithRoomID> CurrentCallInvites;
		m_ValidCallInvitesMap.GenerateValueArray(CurrentCallInvites);

		for (auto CallInvite : CurrentCallInvites)
		{
			FDateTime TimeNow = FDateTime::UtcNow();

			if (CallInvite.CallEvent.ExpireTime < TimeNow)
			{
				// ExpiredInvitesCallIDs.Add(CallInviteMap.Key);

				FString CallID = CallInvite.CallEvent.CallID;
				if (CallInvite.CallEvent.Sender == MatrixLoggedInUserID)
				{
					HangupCallInvite(CallInvite.RoomID, CallInvite.CallEvent.CallID);
					m_ValidCallInvitesMap.Remove(CallInvite.CallEvent.CallID);
					UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
				}
				else
				{
					d_EventOnMatrixCallEventProcessed.Broadcast(EMatrixCallEventType::Hangup, CallInvite.RoomID, CallInvite.CallEvent);
					m_ValidCallInvitesMap.Remove(CallInvite.CallEvent.CallID);
				}
			}
		}
	}
}
//


//Matrix
void TesserconSession::EventLogin()
{
	p_ChatClient = static_cast<FMatrixCoreModule*>(&FModuleManager::Get().LoadModuleChecked(TEXT("MatrixCore")))->GetMatrixClient();
	if (p_ChatClient != nullptr)
	{
		p_ChatClient->Login(m_MatrixUsername, m_LoggedInUserPassword);
		BindMatrixSessionHandlers(true, p_ChatClient->GetSession());
	}
}



void TesserconSession::BindMatrixSessionHandlers(bool DoBind, MatrixSession& Session)
{
	if (DoBind && !bMatrixSessionHandlersBound)
	{
		Session.d_EventOnLoginStateChange.AddRaw(this, &TesserconSession::OnMatrixLoginStateChanged);
		Session.d_EventOnTeamRemoved.AddRaw(this, &TesserconSession::OnTeamRemoved);
		Session.d_EventOnFriendRemoved.AddRaw(this, &TesserconSession::OnFriendRemoved);
		//Call Event Bind
		BindMatrixCallingEvents(true, Session);
		bMatrixSessionHandlersBound = true;
	}
	else if (!DoBind && bMatrixSessionHandlersBound)
	{
		Session.d_EventOnLoginStateChange.RemoveAll(this);
		Session.d_EventOnTeamRemoved.RemoveAll(this);
		Session.d_EventOnFriendRemoved.RemoveAll(this);
		//Call Event Bind
		BindMatrixCallingEvents(true, Session);
		bMatrixSessionHandlersBound = false;
	}
}



void TesserconSession::OnTeamRemoved(FString RoomID)
{
	//  leave call
	if (m_CallState.RoomID == RoomID && m_CallState.CallStatus == ETesserconCallStatus::OnCall)
	{
		LeaveVivoxVoiceCallChannel();
		UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
	}
}

void TesserconSession::OnFriendRemoved(FString RoomID)
{
	//  leave call
	if (m_CallState.RoomID == RoomID && m_CallState.CallStatus == ETesserconCallStatus::OnCall)
	{
		LeaveVivoxVoiceCallChannel();
		UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
	}
}

void TesserconSession::OnMatrixLoginStateChanged(ELoginState State)
{
	FString EventPublicRoom = "!irlFhoFyPGKfvLWsSe:comms.tessercon.com";
	TMap<FString, TSharedPtr<MatrixRoomSession>> GroupRooms = p_ChatClient->GetSession().GetTeamRooms();
	TSharedPtr<MatrixRoomSession> RoomSession = p_ChatClient->GetSession().GetRoomSession(EventPublicRoom);
	if (RoomSession != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT(" In TesserconSession::Room already Join -------------------"));
	}
	else
	{
		if (p_ChatClient != nullptr)
		{
			MatrixNativeAPI::FOnMatrixAPIResponseDelegate AcceptRoomResponseCallback;
			AcceptRoomResponseCallback.BindLambda([this](EMatrixCoreError Error) {
				if (Error == EMatrixCoreError::Success)
				{
					UE_LOG(LogTemp, Warning, TEXT(" In TesserconSession::RoomJoin -------------------"));
				}
				});
			p_ChatClient->GetSession().JoinRoom(EventPublicRoom, false, " ", AcceptRoomResponseCallback);
		}
	}
}

void TesserconSession::PlaceCallBasic(const FString& RoomID)
{
	if (p_ChatClient != nullptr)
	{
		if (p_ChatClient->GetSession().IsRoomDirectConnection(RoomID))
		{
			const FMatrixCallInviteWithRoomID ExistingValidInvite = GetValidCallInviteForRoom(RoomID);
			if (ExistingValidInvite.CallEvent.CallID != "")
			{
				// Glare Condition when there is an existing invite at the place of call --- answering the invite
				AnswerCall(ExistingValidInvite.RoomID, ExistingValidInvite.CallEvent);
			}
			else
			{
				SendMatrixCallInvite(RoomID);
			}
		}
		else
		{
			// update state and connect to vivox channel
			// TODO: update the status to  Connecting??
			UpdateCallState(FTesserconCallState(RoomID, "", ECallType::Team, ETesserconCallStatus::Connecting));

			PlaceCallVivox(RoomID, "", ECallType::Team);
		}

	}
	else
	{
		UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
	}
}

FMatrixCallInviteWithRoomID TesserconSession::GetValidCallInviteForRoom(const FString& RoomID)
{
	for (auto CallInvitePair : m_ValidCallInvitesMap)
	{
		if (CallInvitePair.Value.RoomID == RoomID)
		{
			return CallInvitePair.Value;
		}
	}

	return FMatrixCallInviteWithRoomID();
}

const IAudioDevice* TesserconSession::GetActiveMic() const
{
	if (p_VivoxVoiceClient != nullptr)
	{
		return &p_VivoxVoiceClient->AudioInputDevices().ActiveDevice();
	}
	return nullptr;
}

const IAudioDevice* TesserconSession::GetActiveSpeaker() const
{
	if (p_VivoxVoiceClient != nullptr)
	{
		return &p_VivoxVoiceClient->AudioOutputDevices().ActiveDevice();
	}
	return nullptr;
}

void TesserconSession::SetActiveMic(const FString& MicName)
{
	if (p_VivoxVoiceClient != nullptr)
	{
		TMap<FString, IAudioDevice*> InputDevices = GetInputAudioDevices()->AvailableDevices();
		for (auto Pair : InputDevices)
		{
			if (Pair.Value->Name() == MicName)
			{
				p_VivoxVoiceClient->AudioInputDevices().SetActiveDevice(*(Pair.Value));
				break;
			}
		}
	}
}

void TesserconSession::SetActiveSpeaker(const FString& SpeakerName)
{
	if (p_VivoxVoiceClient != nullptr)
	{
		TMap<FString, IAudioDevice*> InputDevices = GetOutputAudioDevices()->AvailableDevices();
		for (auto Pair : InputDevices)
		{
			if (Pair.Value->Name() == SpeakerName)
			{
				p_VivoxVoiceClient->AudioOutputDevices().SetActiveDevice(*(Pair.Value));
				break;
			}
		}
	}
}

IAudioDevices* TesserconSession::GetInputAudioDevices()
{
	if (p_VivoxVoiceClient != nullptr)
	{
		return &p_VivoxVoiceClient->AudioInputDevices();
	}
	return nullptr;
}


void TesserconSession::GetUserInfoByUsername(const FString& InUsername, const TesserconAPI::FOnUserInfoByUserIDResponseDelegate& TheDelegate) const
{
	if (m_NativeAPI != nullptr)
	{
		m_NativeAPI->GetUserInfoForUser(m_Tokens.AccessToken, InUsername, TheDelegate);
	}
}

IAudioDevices* TesserconSession::GetOutputAudioDevices()
{
	if (p_VivoxVoiceClient != nullptr)
	{
		return &p_VivoxVoiceClient->AudioOutputDevices();
	}
	return nullptr;
}

int TesserconSession::GetSpeakerVolume()
{
	if (p_VivoxVoiceClient != nullptr)
	{
		return p_VivoxVoiceClient->AudioOutputDevices().VolumeAdjustment()+50;
	}
	return -1;
}

void TesserconSession::SetSpeakerVolume(float InVolume)
{
	if (p_VivoxVoiceClient != nullptr)
	{
		p_VivoxVoiceClient->AudioOutputDevices().SetVolumeAdjustment(InVolume - 50);
	}
}

void TesserconSession::PlaceCall(const FString& RoomID)
{
	// check existing call state
	// if processing - this case should not happen incase if the events are not bound correctly handled the exceptions
	if (m_CallState.CallStatus == ETesserconCallStatus::Processing)
	{
		return;
	}
	// if on call
	else if (m_CallState.CallStatus == ETesserconCallStatus::OnCall)
	{
		FOnTesserconSessionResponseDelegate OnEndingCallCallback;
		OnEndingCallCallback.BindLambda([this, RoomID](ETesserconError ErrorStatus)
			{
				if (ErrorStatus == ETesserconError::Success)
				{
					UpdateCallState(FTesserconCallState(RoomID, "", ECallType::Individual, ETesserconCallStatus::Switching));
					PlaceCallBasic(RoomID);
				}
			});

		EndCall(true, true, OnEndingCallCallback);
	}

	else if (m_CallState.CallStatus == ETesserconCallStatus::Available)
	{
		PlaceCallBasic(RoomID);
	}
}

void TesserconSession::AnswerCall(const FString& RoomID, const FMatrixCallEvent& CallInvite)
{
	// This only for matrix for now 
	if (m_CallState.CallStatus == ETesserconCallStatus::Processing)
	{
		return;
	}

	// if on call
	else if (m_CallState.CallStatus == ETesserconCallStatus::OnCall)
	{
		FOnTesserconSessionResponseDelegate OnEndingCallCallback;
		OnEndingCallCallback.BindLambda([this, RoomID, CallInvite](ETesserconError ErrorStatus)
			{
				if (ErrorStatus == ETesserconError::Success)
				{
					// TODO: update swithcing state
					UpdateCallState(FTesserconCallState(RoomID, CallInvite.CallID, ECallType::Individual, ETesserconCallStatus::Switching));
					SendMatrixCallAnswer(RoomID, CallInvite.CallID);
				}
			});

		EndCall(true, true, OnEndingCallCallback);
	}

	else if (m_CallState.CallStatus == ETesserconCallStatus::Available || m_CallState.CallStatus == ETesserconCallStatus::Switching) // TODO: switching also
	{
		SendMatrixCallAnswer(RoomID, CallInvite.CallID);
	}
}

void TesserconSession::EndCall()
{
	EndCall(false);
}

void TesserconSession::HangupCallInvite(const FString& RoomID, const FString& CallID, const FOnTesserconSessionResponseDelegate& TheDelegate)
{
	// TODO: check if this needs to included in SendMatrixCallHangup function
	if (p_ChatClient != nullptr)
	{
		MatrixNativeAPI::FOnMatrixAPIResponseDelegate OnHangupCallback;
		OnHangupCallback.BindLambda([this, TheDelegate](EMatrixCoreError ErrorStatus)
			{
				if (ErrorStatus == EMatrixCoreError::Success)
				{
					TheDelegate.ExecuteIfBound(ETesserconError::Success);
				}
				else
				{
					TheDelegate.ExecuteIfBound(ETesserconError::Error);
				}
			});
		p_ChatClient->GetSession().SendCallHangup(RoomID, CallID, OnHangupCallback);
	}
	else
	{
		TheDelegate.ExecuteIfBound(ETesserconError::Error);
	}
}

void TesserconSession::SendMatrixCallInvite(const FString& RoomID)
{
	if (p_ChatClient != nullptr)
	{
		FString CallID = GenerateTransactionID(MATRIX_CALL_ID_LENGTH);

		UpdateCallState(FTesserconCallState(RoomID, CallID, ECallType::Individual, ETesserconCallStatus::Processing));

		MatrixNativeAPI::FOnMatrixAPIResponseDelegate OnResponseCallback;

		OnResponseCallback.BindLambda([this, RoomID, CallID](EMatrixCoreError ErrorStatus)
			{
				if (ErrorStatus == EMatrixCoreError::Success)
				{
					UpdateCallState(FTesserconCallState(RoomID, CallID, ECallType::Individual, ETesserconCallStatus::Dialing));
				}
				else
				{
					UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
				}

			});

		p_ChatClient->GetSession().SendCallInvite(RoomID, CallID, OnResponseCallback);
	}
	else
	{
		UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
	}
}

void TesserconSession::SendMatrixCallAnswer(const FString& RoomID, const FString& CallID)
{
	if (p_ChatClient != nullptr)
	{
		UpdateCallState(FTesserconCallState(RoomID, CallID, ECallType::Individual, ETesserconCallStatus::Processing));

		MatrixNativeAPI::FOnMatrixAPIResponseDelegate OnResponseCallback;
		OnResponseCallback.BindLambda([this, RoomID, CallID](EMatrixCoreError ErrorStatus)
			{
				if (ErrorStatus == EMatrixCoreError::Success)
				{
					// remove from valid call invites
					RemoveEventFromValidCallInvites(CallID);
					UpdateCallState(FTesserconCallState(RoomID, CallID, ECallType::Individual, ETesserconCallStatus::Connecting));
					PlaceCallVivox(RoomID, CallID, ECallType::Individual);
				}
				else
				{
					UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
				}
			});

		p_ChatClient->GetSession().SendCallAnswer(RoomID, CallID, OnResponseCallback);
	}
	else
	{
		UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
	}
}

void TesserconSession::SendMatrixCallHangup(const FString& RoomID, const FString& CallID, const FOnTesserconSessionResponseDelegate& TheDelegate)
{
	// end matrix call and handle the call state
	if (p_ChatClient != nullptr)
	{
		MatrixNativeAPI::FOnMatrixAPIResponseDelegate OnResponseCallback;

		OnResponseCallback.BindLambda([this, TheDelegate/*, RoomID, CallID*/](EMatrixCoreError ErrorStatus)
			{
				if (ErrorStatus == EMatrixCoreError::Success)
				{
					// updatecallstate
					TheDelegate.ExecuteIfBound(ETesserconError::Success);
				}
				else
				{
					// always called after the vivox channel is removed
					UpdateCallState(FTesserconCallState(ETesserconCallStatus::Available));
					TheDelegate.ExecuteIfBound(ETesserconError::Error);
				}
			});

		p_ChatClient->GetSession().SendCallHangup(RoomID, CallID, OnResponseCallback);
	}
}

void TesserconSession::BindMatrixCallingEvents(bool bDoBind, MatrixSession& MSession)
{
	if (bDoBind)
		MSession.d_EventOnCallEventReceived.AddRaw(this, &TesserconSession::OnMatrixCallEventReceived);

	else
		MSession.d_EventOnCallEventReceived.RemoveAll(this);
}

void TesserconSession::SetSpeakerMuted(bool bIsEnabled)
{
	if (p_VivoxVoiceClient == nullptr) return;

	IAudioDevices& OutputDevices = p_VivoxVoiceClient->AudioOutputDevices();

	OutputDevices.SetMuted(bIsEnabled);
}

void TesserconSession::SetMicMuted(bool bIsEnabled)
{
	if (p_VivoxVoiceClient == nullptr) return;

	IAudioDevices& InputDevices = p_VivoxVoiceClient->AudioInputDevices();

	InputDevices.SetMuted(bIsEnabled);
}

bool TesserconSession::GetSpeakerMuted()
{
	if (p_VivoxVoiceClient == nullptr) return false;

	IAudioDevices& OutputDevices = p_VivoxVoiceClient->AudioOutputDevices();

	return OutputDevices.Muted();
}

bool TesserconSession::GetMicMuted()
{
	if (p_VivoxVoiceClient == nullptr) return false;

	IAudioDevices& InputDevices = p_VivoxVoiceClient->AudioInputDevices();

	return InputDevices.Muted();
}


//NearBy

VivoxCoreError TesserconSession::SendTextMessage(const FString& ChannelName, const FString& Message, EVivoxChannelKey ChannelKey, const FOnVivoxAPIResponseDelegate& TheDelegate)
{
	if (p_VivoxVoiceClient == nullptr) return VxErrorNotInitialized;

	// TODO: harcoding global channel for now  - Get Channel Name by Key

	ILoginSession& LoginSession = p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID);
	IChannelSession& ChannelSession = LoginSession.GetChannelSession(m_VivoxNearByChannel);

	/*if (ChannelKey == EVivoxChannelKey::NearByChannel)
	{
		
	}*/
	/*else if (ChannelKey == EVivoxChannelKey::VoiceCallChannel)
	{
		ChannelSession = LoginSession.GetChannelSession(m_VivoxGlobalChannel);
	}*/


	IChannelSession::FOnBeginSendTextCompletedDelegate SendChannelMessageCallback;
	SendChannelMessageCallback.BindLambda([this, ChannelName, Message, TheDelegate](VivoxCoreError Error)
		{
			if (Error == VxErrorSuccess)
			{
				UE_LOG(LogTemp, Log, TEXT("Message sent to %s: %s\n"), *ChannelName, *Message);
			}

			TheDelegate.ExecuteIfBound(Error);
		});

	return ChannelSession.BeginSendText(Message, SendChannelMessageCallback);
}

IChannelSession& TesserconSession::GetChannelSessionFromKey(EVivoxChannelKey Key)
{
	// TODO:
	//if (p_VivoxVoiceClient == nullptr) return NULL;

	ILoginSession& LoginSession = p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID);

	if (Key == EVivoxChannelKey::GlobalTextOnlyChannel)
	{
		return LoginSession.GetChannelSession(m_VivoxGlobalChannel);
	}
	else if (Key == EVivoxChannelKey::NearByChannel)
	{
		return LoginSession.GetChannelSession(m_VivoxNearByChannel);
	}
	else if (Key == EVivoxChannelKey::VoiceCallChannel)
	{
		return LoginSession.GetChannelSession(m_VivoxVoiceCallChannel);
	}
	else
	{
		return LoginSession.GetChannelSession(ChannelId());
	}
}

TSharedPtr<IChannelSession> TesserconSession::GetChannelSessionFromName(const FString& ChannelName)
{
	if (p_VivoxVoiceClient == nullptr) return NULL;

	ILoginSession& LoginSession = p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID);
	TSharedPtr<IChannelSession> FoundChannelSession;

	for (auto& ChannelSession : LoginSession.ChannelSessions())
	{
		if (ChannelSession.Key.Name() == ChannelName)
		{
			FoundChannelSession = ChannelSession.Value;
			break;
		}
	}

	return FoundChannelSession;
}



void TesserconSession::UnSetNearByChannelTransmissionMode()
{
	bool bIscheck = true;
	IChannelSession& ChannelSession = GetChannelSessionFromKey(EVivoxChannelKey::NearByChannel);

	IChannelSession::FOnBeginSetAudioConnectedCompletedDelegate AudioSetCallBack;
	AudioSetCallBack.BindLambda([this, bIscheck](VivoxCoreError Status)
		{	if (Status == VxErrorSuccess)
		{
			
			 p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID).SetTransmissionMode(TransmissionMode::None, m_VivoxNearByChannel);
		}
			});
	ChannelSession.BeginSetAudioConnected(false, true, AudioSetCallBack);
}


void TesserconSession::SetNearByChannelTransmissionMode()
{
	
	IChannelSession& ChannelSession = GetChannelSessionFromKey(EVivoxChannelKey::NearByChannel);
	IChannelSession::FOnBeginSetAudioConnectedCompletedDelegate AudioSetCallBack;
	AudioSetCallBack.BindLambda([this](VivoxCoreError Status)
		{ 
			if (Status == VxErrorSuccess)
			{
				
				p_VivoxVoiceClient->GetLoginSession(m_VivoxLoggedInAccountID).SetTransmissionMode(TransmissionMode::Single, m_VivoxNearByChannel);
			}
		});
	ChannelSession.BeginSetAudioConnected(true, true, AudioSetCallBack);

}