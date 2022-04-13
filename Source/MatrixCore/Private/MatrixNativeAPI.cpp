#include "MatrixNativeAPI.h"
#include "MatrixResponses.h"

#include "Json.h"
#include "JsonUtilities.h"

MatrixNativeAPI::MatrixNativeAPI()
{
	UE_LOG(LogMatrixCore, Warning, TEXT(" In MatrixNativeAPI::MatrixNativeAPI() -------------------"));
	p_HttpModule = &FHttpModule::Get();

}

MatrixNativeAPI::~MatrixNativeAPI()
{
	UE_LOG(LogMatrixCore, Warning, TEXT(" In MatrixNativeAPI::~MatrixNativeAPI() -------------------"));
}

FString MatrixNativeAPI::GetRequestContentAsString(FHttpRequestPtr Request)
{
	FUTF8ToTCHAR TCHARData(reinterpret_cast<const ANSICHAR*>(Request->GetContent().GetData()), Request->GetContentLength());
	return FString(TCHARData.Length(), TCHARData.Get());
}


void MatrixNativeAPI::Login(const FString& Username, const FString& Password)
{
	FString LoginUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/login";

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
	RequestObj->SetStringField("password", Password);
	RequestObj->SetStringField("user", Username);
	RequestObj->SetStringField("type", "m.login.password");

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		// UE_LOG(LogMatrixCore, Warning, TEXT("MatrixNativeAPI::Login %s"), *RequestBody);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixLoginRequest = p_HttpModule->CreateRequest();
		MatrixLoginRequest->OnProcessRequestComplete().BindRaw(this, &MatrixNativeAPI::OnMatrixLoginResponseReceived);
		MatrixLoginRequest->SetURL(LoginUrl);
		MatrixLoginRequest->SetVerb("POST");
		MatrixLoginRequest->SetHeader("Content-Type", "application/json");
		MatrixLoginRequest->SetContentAsString(RequestBody);
		MatrixLoginRequest->ProcessRequest();
	}
}

void MatrixNativeAPI::OnMatrixLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	MatrixLoginResponse LoginResponse;

	if (bWasSuccessful)
	{
		UE_LOG(LogMatrixCore, Warning, TEXT("MatrixNativeAPI::OnMatrixLoginResponseReceived  -- response code = %d content = %s"), Response->GetResponseCode(),
												*Response->GetContentAsString());
		
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{

			if (JsonObject->HasField("access_token") && JsonObject->HasField("user_id") && JsonObject->HasField("device_id") && JsonObject->HasField("home_server"))
			{
				LoginResponse.AccessToken = JsonObject->GetStringField("access_token");
				LoginResponse.DeviceID = JsonObject->GetStringField("device_id");
				LoginResponse.UserID = JsonObject->GetStringField("user_id");
				LoginResponse.HomeServer = JsonObject->GetStringField("home_server");

				d_OnLoginResponseReceived.ExecuteIfBound(EMatrixCoreError::Success, LoginResponse, "");
			}

			else if (JsonObject->HasField("error") && JsonObject->HasField("errcode"))
			{
				d_OnLoginResponseReceived.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")), LoginResponse, JsonObject->GetStringField("error"));

			}

			else
			{
				d_OnLoginResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, LoginResponse, "");
			}
		}
		else
		{
			d_OnLoginResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, LoginResponse, "");

		}
	}
	else
	{
		d_OnLoginResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, LoginResponse, "");

	}
}

void MatrixNativeAPI::Logout(const FString& AccessToken)
{
	FString LogoutUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/logout";

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixLogoutRequest = p_HttpModule->CreateRequest();
	// TODO: change this to lamda delegate
	//MatrixLogoutRequest->OnProcessRequestComplete().BindRaw(this, &MatrixNativeAPI::OnMatrixLogoutResponseReceived);
	MatrixLogoutRequest->SetURL(LogoutUrl);
	MatrixLogoutRequest->SetVerb("POST");
	MatrixLogoutRequest->SetHeader("Content-Type", "application/x-www-form-urlencoded");
	MatrixLogoutRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	MatrixLogoutRequest->ProcessRequest();
}


void MatrixNativeAPI::OnMatrixLogoutResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (Response->GetResponseCode() == 200)
		d_OnLogoutResponseReceived.ExecuteIfBound(EMatrixCoreError::Success, "");
	else
		d_OnLogoutResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "Logout Failed");
}

void MatrixNativeAPI::Sync(const FString& AccessToken, const FString& NextBatchToken, bool bIsInitial)
{
	// sync 
	// TODO: if the intial sync is not true change the api
	// add filters and since options when requesting the request
	// add presence options
	FString SyncUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/sync";
	
	if (!bIsInitial && !NextBatchToken.IsEmpty())
	{
		// TODO:
		SyncUrl += "?since=" + NextBatchToken;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixSyncRequest = p_HttpModule->CreateRequest();
	MatrixSyncRequest->OnProcessRequestComplete().BindRaw(this, &MatrixNativeAPI::OnMatrixSyncResponseReceived);
	MatrixSyncRequest->SetURL(SyncUrl);
	MatrixSyncRequest->SetVerb("GET");
	MatrixSyncRequest->SetHeader("Content-Type", "application/x-www-form-urlencoded");
	MatrixSyncRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	MatrixSyncRequest->ProcessRequest();
}

void MatrixNativeAPI::OnMatrixSyncResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	MatrixSyncResponse SyncResponse;

	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (JsonObject->HasField("next_batch"))
			{
				// to know if this is intial sync
				// TODO: add a pram in the sync delegate mentioning it
				bool bIsIntialSync = false;
				FString NextBatchToken = Request->GetURLParameter("since");

				if (NextBatchToken == "")
				{
					bIsIntialSync = true;
				}
				
				SyncResponse.NextBatch = JsonObject->GetStringField("next_batch");

				if (JsonObject->HasField("account_data"))
				{

					// reading account data
					TSharedPtr<FJsonObject> AccountObject = JsonObject->GetObjectField("account_data");
					if (AccountObject->HasField("events"))
					{
						for (TSharedPtr<FJsonValue> Event : AccountObject->GetArrayField("events"))
						{
							TSharedPtr<FJsonObject> EventObject = Event->AsObject();
							if (EventObject->HasField("type"))
							{
								/*if (EventObject->GetStringField("type") == "m.direct" && EventObject->HasField("content"))
								{
									for (auto UserID_RoomPair : EventObject->GetObjectField("content")->Values)
									{
										FString RoomID = UserID_RoomPair.Value->AsArray()[0]->AsString();
										FString UserID = UserID_RoomPair.Key;
										SyncResponse.AccountData.DirectConnections.Add(UserID, RoomID);
									}
								}*/

								// TODO: convert this to m.direct
								if (EventObject->GetStringField("type") == "comms.tessercon.direct" && EventObject->HasField("content"))
								{
									SyncResponse.AccountData.bHasDirectConnectionsUpdate = true;
									for (auto UserID_RoomPair : EventObject->GetObjectField("content")->Values)
									{
										FString RoomID = UserID_RoomPair.Value->AsString();
										FString UserID = UserID_RoomPair.Key;
										SyncResponse.AccountData.DirectConnections.Add(UserID, RoomID);
									}
								}

								// in sync account data
								//{
								//	"type": "m.ignored_user_list",
								//		"content" : {
								//		"ignored_users": {
								//			"@nagesh:comms.tessercon.com": {}
								//		}
								//	}
								//},
								if (EventObject->GetStringField("type") == "m.ignored_user_list" && EventObject->HasField("content"))
								{
									SyncResponse.AccountData.bHasBlockedUserUpdate = true;
									TSharedPtr<FJsonObject> ContentObject = EventObject->GetObjectField("content");
									if (ContentObject->HasField("ignored_users"))
									{
										TSharedPtr<FJsonObject> IgnoredUsersObject = ContentObject->GetObjectField("ignored_users");
										for (auto Users : IgnoredUsersObject->Values)
										{
											SyncResponse.AccountData.BlockedUsers.Add(Users.Key);
										}
									}
								}
							}
						}
					}
				}
				if (JsonObject->HasField("presence"))
				{

					// reading presence data
					TSharedPtr<FJsonObject> PresenceObject = JsonObject->GetObjectField("presence");
					if (PresenceObject->HasField("events"))
					{
						for (TSharedPtr<FJsonValue> Event : PresenceObject->GetArrayField("events"))
						{
							TSharedPtr<FJsonObject> EventObject = Event->AsObject();
							if (EventObject->HasField("type"))
							{
								if (EventObject->GetStringField("type") == "m.presence" && EventObject->HasField("sender") && EventObject->HasField("content"))
								{
									FString Sender = EventObject->GetStringField("sender");
									EMatrixPresence Presence;
									if (EventObject->GetObjectField("content")->HasField("presence"))
									{
										Presence = ConvertStringToMatrixPresence(EventObject->GetObjectField("content")->GetStringField("presence"));
									}
									SyncResponse.PresenceMap.Add(Sender, Presence);
								}
							}
						}
					}

				}

				if (JsonObject->HasField("rooms"))
				{
					// room invite
					TSharedPtr<FJsonObject> RoomsObject = JsonObject->GetObjectField("rooms");

					if (RoomsObject->HasField("invite") && RoomsObject->HasField("leave") && RoomsObject->HasField("join"))
					{
						// invites 
						TSharedPtr<FJsonObject> RoomsInviteObject = RoomsObject->GetObjectField("invite");

						for (auto Invite : RoomsInviteObject->Values)
						{
							FString RoomID = Invite.Key;

							FMatrixInvitedRoom InvitedRoom;

							TSharedPtr<FJsonObject> InviteObject = Invite.Value->AsObject();

							if (InviteObject->HasField("invite_state"))
							{
								TSharedPtr<FJsonObject> InviteStateObj = InviteObject->GetObjectField("invite_state");

								if (InviteStateObj->HasField("events"))
								{
									for (auto Event : InviteStateObj->GetArrayField("events"))
									{
										if (Event->AsObject()->HasField("type"))
										{
											if (Event->AsObject()->GetStringField("type") == "m.room.member" && Event->AsObject()->HasField("content") &&
												Event->AsObject()->HasField("state_key") && Event->AsObject()->HasField("sender"))
											{
												FString Receiver = Event->AsObject()->GetStringField("state_key");
												FString Sender = Event->AsObject()->GetStringField("sender");
												TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
												if (ContentObject->HasField("membership") && ContentObject->GetStringField("membership") == "invite")
												{
													bool bIsDirect = false;
													if (ContentObject->HasField("is_direct"))
													{
														bIsDirect = ContentObject->GetBoolField("is_direct");
													}
													InvitedRoom.Receiver = Receiver;
													InvitedRoom.RoomID = RoomID;
													InvitedRoom.Sender = Sender;
													InvitedRoom.bIsDirect = bIsDirect;
												}
											}

											if (Event->AsObject()->GetStringField("type") == "m.room.name" && Event->AsObject()->HasField("content"))
											{
												TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
												if (ContentObject->HasField("name"))
												{
													InvitedRoom.Name = ContentObject->GetStringField("name");
												}
											}
										}
									}
								}

								SyncResponse.Rooms.InvitedRooms.Add(InvitedRoom);
							}
						}

						// leave
						TSharedPtr<FJsonObject> RoomsLeaveObject = RoomsObject->GetObjectField("leave");
						for (auto Room : RoomsLeaveObject->Values)
						{
							FString RoomID = Room.Key;
							SyncResponse.Rooms.LeftRooms.Add(RoomID);
						}

						//  join
						TSharedPtr<FJsonObject> RoomsJoinedObject = RoomsObject->GetObjectField("join");
						for (auto Room : RoomsJoinedObject->Values)
						{
							FMatrixJoinedRoomSync JoinedRoom;
							//JoinedRoom.RoomID = Room.Key;
														
							TSharedPtr<FJsonObject> JoinedRoomObject = Room.Value->AsObject();

							if (JoinedRoomObject->HasField("timeline"))
							{
								TSharedPtr<FJsonObject> TimelineObject = JoinedRoomObject->GetObjectField("timeline");
								
								if (TimelineObject->HasField("events"))
								{
									JoinedRoom.RoomEvents = ProcessEventsForJoinedRoom(TimelineObject->GetArrayField("events"));

								}

								if (TimelineObject->HasField("prev_batch"))
								{
									JoinedRoom.PrevBatch = TimelineObject->GetStringField("prev_batch");
								}
							}

							if (JoinedRoomObject->HasField("state"))
							{
								TSharedPtr<FJsonObject> StateObject = JoinedRoomObject->GetObjectField("state");

								if (StateObject->HasField("events"))
								{
									for (auto Event : StateObject->GetArrayField("events"))
									{
										if (Event->AsObject()->HasField("type") && Event->AsObject()->HasField("content"))
										{
											FString EventType = Event->AsObject()->GetStringField("type");

											if (EventType == "m.room.name")
											{
												TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
												if (ContentObject->HasField("name"))
												{
													JoinedRoom.RoomEvents.RoomName = ContentObject->GetStringField("name");
												}
											}

											// TODO: power_levels
											/*{
												"type": "m.room.power_levels",
													"sender" : "@chaitanya:comms.tessercon.com",
													"content" : {
													"users": {
														"@chaitanya:comms.tessercon.com": 100
													},
														"users_default" : 0,
															"events" : {
															"m.room.name": 50,
																"m.room.power_levels" : 100,
																"m.room.history_visibility" : 100,
																"m.room.canonical_alias" : 50,
																"m.room.avatar" : 50,
																"m.room.tombstone" : 100,
																"m.room.server_acl" : 100,
																"m.room.encryption" : 100
														},
															"events_default": 0,
																"state_default" : 50,
																"ban" : 50,
																"kick" : 50,
																"redact" : 50,
																"invite" : 0
												},
													"state_key": "",
																"origin_server_ts" : 1617983127653,
																"unsigned" : {
																"age": 16579047320
															},
																"event_id" : "$WIynSOOyhYjEmnpbXUKXB7L5h-XRRJGMkNlkRsn1WOk"
											}*/

											if (EventType == "m.room.power_levels")
											{
												TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
												if (ContentObject->HasField("users"))
												{
													TSharedPtr<FJsonObject> UsersObject = ContentObject->GetObjectField("users");
													
													for (auto PowerValues : UsersObject->Values)
													{
														FString UserID = PowerValues.Key;
														int PowerLevel = PowerValues.Value->AsNumber();
														if (PowerLevel == 100)
														{
															JoinedRoom.RoomEvents.Admins.Add(UserID);
														}
													}
												}
											}

											// TODO:
											if (Event->AsObject()->GetStringField("type") == "m.room.member" && Event->AsObject()->HasField("Content") && 
												Event->AsObject()->HasField("state_key"))
											{
												FString UserID = Event->AsObject()->GetStringField("state_key");
												TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");

												if (ContentObject->HasField("membership"))
												{
													EMatrixMembership Membership = ConvertStringToMatrixMembership(ContentObject->GetStringField("membership"));
													JoinedRoom.RoomEvents.MemberStatus.Add(UserID, Membership);
												}
											}
										}
									}
								}
							}

							if (JoinedRoomObject->HasField("ephemeral"))
							{
								TSharedPtr<FJsonObject> EphemeralObject = JoinedRoomObject->GetObjectField("ephemeral");

								if (EphemeralObject->HasField("events"))
								{
									for (auto Event : EphemeralObject->GetArrayField("events"))
									{
										if (Event->AsObject()->HasField("type") && Event->AsObject()->HasField("content"))
										{
											FString EventType = Event->AsObject()->GetStringField("type");
											// TODO: could also include m.receipt for now skipping it for fully read
											if (EventType == "m.typing")
											{
												TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
												if (ContentObject->HasField("user_ids"))
												{
													for (auto UserValue : ContentObject->GetArrayField("user_ids"))
													{
														JoinedRoom.RoomEvents.TypingUsers.Add(UserValue->AsString());
													}
												}
											}
										}
									}
								}
							}

							if (JoinedRoomObject->HasField("account_data"))
							{
								TSharedPtr<FJsonObject> RoomAccountDataObject = JoinedRoomObject->GetObjectField("account_data");

								if (RoomAccountDataObject->HasField("events"))
								{
									for (auto Event : RoomAccountDataObject->GetArrayField("events"))
									{
										if (Event->AsObject()->HasField("type") && Event->AsObject()->HasField("content"))
										{
											FString EventType = Event->AsObject()->GetStringField("type");

											if (EventType == "m.fully_read")
											{
												TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
												if (ContentObject->HasField("event_id"))
												{
													JoinedRoom.RoomEvents.FullyReadReceipt = ContentObject->GetStringField("event_id");
												}
											}
										}
									}
								}
							}

							JoinedRoom.RoomID = Room.Key;

							SyncResponse.Rooms.JoinedRooms.Add(JoinedRoom);
						}


					}					
				}

				d_OnSyncResponseReceived.ExecuteIfBound(EMatrixCoreError::Success, SyncResponse, bIsIntialSync, "");
			}
			

			else if (JsonObject->HasField("error") && JsonObject->HasField("errcode"))
			{
				d_OnSyncResponseReceived.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")), SyncResponse, false,JsonObject->GetStringField("error"));
			}

			else
			{
				d_OnSyncResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, SyncResponse, false, "");
			}
		}
		else
		{
			d_OnSyncResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, SyncResponse,false, "");
		}
	}
	else
	{
		d_OnSyncResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, SyncResponse, false, "");
	}
}

void MatrixNativeAPI::CreateRoom(const FString& AccessToken, const TArray<FString>& UserIDs, bool bIsDirect, const FString& RoomName, const FOnMatrixCreateRoomReponseDelegate& TheDelegate)
{
	if (UserIDs.Num() > 0)
	{
		FString CreateRoomUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/createRoom";

		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("visibility", "private");
		
		TArray<TSharedPtr<FJsonValue>> UserIDJsonValues;

		if (bIsDirect)
		{
			RequestObj->SetStringField("is_direct", "true");
			//RequestObj->SetStringField("preset", "trusted_private_chat");
			RequestObj->SetStringField("preset", "private_chat");
			UserIDJsonValues.Add(MakeShareable(new FJsonValueString(UserIDs[0])));
		}
		else
		{
			RequestObj->SetStringField("name", RoomName);
			for (FString ID : UserIDs)
			{
				UserIDJsonValues.Add(MakeShareable(new FJsonValueString(ID)));
			}
		}

		RequestObj->SetArrayField("invite", UserIDJsonValues);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
		{
			// UE_LOG(LogMatrixCore, Warning, TEXT("MatrixNativeAPI::Login %s"), *RequestBody);
			TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixCreateRoomRequest = p_HttpModule->CreateRequest();
			MatrixCreateRoomRequest->OnProcessRequestComplete().BindLambda([this, TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
			{
				OnMatrixCreateRoomResponseReceived(Request, Response, bWasSuccessful, TheDelegate);
			});
			MatrixCreateRoomRequest->SetURL(CreateRoomUrl);
			MatrixCreateRoomRequest->SetVerb("POST");
			MatrixCreateRoomRequest->SetHeader("Content-Type", "application/json");
			MatrixCreateRoomRequest->SetHeader("Authorization", "Bearer " + AccessToken);
			MatrixCreateRoomRequest->SetContentAsString(RequestBody);
			MatrixCreateRoomRequest->ProcessRequest();
		}
	}

}

void MatrixNativeAPI::OnMatrixCreateRoomResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnMatrixCreateRoomReponseDelegate TheDelegate)
{
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{

			if (JsonObject->HasField("room_id"))
			{
				FString RoomID = JsonObject->GetStringField("room_id");
				FString RequestBody = GetRequestContentAsString(Request);
				TSharedPtr<FJsonObject> JsonRequestObject;
				TSharedRef<TJsonReader<>> RequestReader = TJsonReaderFactory<>::Create(RequestBody);
				if (FJsonSerializer::Deserialize(RequestReader, JsonRequestObject))
				{
					if (JsonRequestObject->HasField("is_direct") && JsonRequestObject->HasField("invite"))
					{
						// call the account update API
						// send a flag so that on lamda from matrix session can call the update API
						bool bIsDirect = JsonRequestObject->GetBoolField("is_direct");
						FString UserID;
						if (bIsDirect)
						{
							UserID = JsonRequestObject->GetArrayField("invite")[0]->AsString();
						}
						d_OnCreateRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::Success, RoomID, bIsDirect, UserID, "");
						TheDelegate.ExecuteIfBound(EMatrixCoreError::Success, RoomID);
					}
					else
					{
						d_OnCreateRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::Success, RoomID, false, "", "");
						TheDelegate.ExecuteIfBound(EMatrixCoreError::Success, RoomID);
					}
				}

				else
				{
					d_OnCreateRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::Success, RoomID, false, "", "");
					TheDelegate.ExecuteIfBound(EMatrixCoreError::Success, RoomID);
				}
			}

			else if (JsonObject->HasField("error") && JsonObject->HasField("errcode"))
			{
				d_OnCreateRoomResponseReceived.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")),  "", false, "", JsonObject->GetStringField("error"));
				TheDelegate.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")), "");

			}

			else
			{
				d_OnCreateRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "", false, "", "");
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "");
			}
		}
		else
		{
			d_OnCreateRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "", false, "", "");
			TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "");
		}
	}
	else
	{
		d_OnCreateRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "", false, "","");
		TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "");
	}
}

void MatrixNativeAPI::InviteToRoom(const FString& AccessToken, const FString& UserID, const FString& RoomID)
{
	FString InviteUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/invite";

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
	RequestObj->SetStringField("user_id", UserID);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixInviteRoomRequest = p_HttpModule->CreateRequest();
		// MatrixInviteRoomRequest->OnProcessRequestComplete().BindRaw(this, &MatrixNativeAPI::);
		MatrixInviteRoomRequest->SetURL(InviteUrl);
		MatrixInviteRoomRequest->SetVerb("POST");
		MatrixInviteRoomRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		MatrixInviteRoomRequest->SetHeader("Content-Type", "application/json");
		MatrixInviteRoomRequest->SetContentAsString(RequestBody);
		MatrixInviteRoomRequest->ProcessRequest();
	}
}

void MatrixNativeAPI::JoinRoom(const FString& AccessToken, const FString& RoomID, const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString JoinUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/join";

	// UE_LOG(LogMatrixCore, Warning, TEXT("MatrixNativeAPI::Login %s"), *RequestBody);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixJoinRoomRequest = p_HttpModule->CreateRequest();
	MatrixJoinRoomRequest->OnProcessRequestComplete().BindLambda([this, TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		OnMatrixJoinRoomResponseReceived(Request, Response, bWasSuccessful, TheDelegate);
	});//BindRaw(this, &MatrixNativeAPI::OnMatrixJoinRoomResponseReceived);
	MatrixJoinRoomRequest->SetURL(JoinUrl);
	MatrixJoinRoomRequest->SetVerb("POST");
	MatrixJoinRoomRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	MatrixJoinRoomRequest->ProcessRequest();

}

void MatrixNativeAPI::OnMatrixJoinRoomResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnMatrixAPIResponseDelegate TheDelegate)
{
	UE_LOG(LogMatrixCore, Warning, TEXT("MatrixNativeAPI::OnMatrixJoinRoomResponseReceived response code = %d , response content = %s"), Response->GetResponseCode(), *Response->GetContentAsString());
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (JsonObject->HasField("room_id"))
			{
				d_OnJoinRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::Success, JsonObject->GetStringField("room_id"), "");
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}

			else if (JsonObject->HasField("error") && JsonObject->HasField("errcode"))
			{
				d_OnJoinRoomResponseReceived.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")), "", JsonObject->GetStringField("error"));
				TheDelegate.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")));
			}
			else
			{
				d_OnJoinRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "",  "");
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		}
		else
		{
			d_OnJoinRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "", "");
			TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
		}
	}
	else
	{
		d_OnJoinRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "", "");
		TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
	}
}

void MatrixNativeAPI::LeaveRoom(const FString& AccessToken, const FString& RoomID, const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString LeaveUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/leave";

	// UE_LOG(LogMatrixCore, Warning, TEXT("MatrixNativeAPI::Login %s"), *RequestBody);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixForgetRoomRequest = p_HttpModule->CreateRequest();
	MatrixForgetRoomRequest->OnProcessRequestComplete().BindLambda([this, TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		OnMatrixLeaveRoomResponseReceived(Request, Response, bWasSuccessful, TheDelegate);
	}); //BindRaw(this, &MatrixNativeAPI::OnMatrixLeaveRoomResponseReceived);
	MatrixForgetRoomRequest->SetURL(LeaveUrl);
	MatrixForgetRoomRequest->SetVerb("POST");
	MatrixForgetRoomRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	MatrixForgetRoomRequest->ProcessRequest();
}

void MatrixNativeAPI::OnMatrixLeaveRoomResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnMatrixAPIResponseDelegate TheDelegate)
{
	if (bWasSuccessful)
	{
		if (Response->GetResponseCode() == 200)
		{
			FString RequestUrl = Request->GetURL();
			FString RoomID;
			FString Right, LeftTemp;
			RequestUrl.Split(TEXT("rooms/"), &LeftTemp, &Right);
			Right.Split(TEXT("/leave"), &RoomID, &LeftTemp);

			d_OnLeaveRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::Success, RoomID, "");
			TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
		}
		else
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				if (JsonObject->HasField("error") && JsonObject->HasField("errcode"))
				{
					d_OnLeaveRoomResponseReceived.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")), "", JsonObject->GetStringField("error"));
					TheDelegate.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")));
				}

				else
				{
					d_OnLeaveRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "", "");
					TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
				}
			}
			else
			{
				d_OnLeaveRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "", "");
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		}
	}
	else
	{
		d_OnLeaveRoomResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, "", "");
		TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
	}
}

void MatrixNativeAPI::UpdateDirectConnections(const FString& AccessToken, const TMap<FString, FString>& UserRoomPair, const FString& UserID, const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString AccountDataUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/user/" + UserID + "/account_data/comms.tessercon.direct";

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);

	for (auto Pair : UserRoomPair)
		RequestObj->SetStringField(Pair.Key, Pair.Value);
	
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		// UE_LOG(LogMatrixCore, Warning, TEXT("MatrixNativeAPI::Login %s"), *RequestBody);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixAccountDataRequest = p_HttpModule->CreateRequest();
		MatrixAccountDataRequest->OnProcessRequestComplete().BindLambda([TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		});//.BindRaw(this, &MatrixNativeAPI::OnMatrixUpdateDirectConnections);
		MatrixAccountDataRequest->SetURL(AccountDataUrl);
		MatrixAccountDataRequest->SetVerb("PUT");
		MatrixAccountDataRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		MatrixAccountDataRequest->SetHeader("Content-Type", "application/json");
		MatrixAccountDataRequest->SetContentAsString(RequestBody);
		MatrixAccountDataRequest->ProcessRequest();
	}	
}

void MatrixNativeAPI::GetRoomMembers(const FString& AccessToken, const FString& RoomID, EMatrixMembership Membership, const FOnMatrixRoomMembersResponseDelegate& TheDelegate)
{
	FString RoomMembersUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/members?membership=" + ConvertMatrixMembershipToString(Membership);
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixRoomMembersRequest = p_HttpModule->CreateRequest();
	MatrixRoomMembersRequest->OnProcessRequestComplete().BindLambda([this, TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		OnMatrixRoomMembersResponseReceived(Request, Response, bWasSuccessful, TheDelegate);
	});//BindRaw(this, &MatrixNativeAPI::OnMatrixRoomMembersResponseReceived);
	MatrixRoomMembersRequest->SetURL(RoomMembersUrl);
	MatrixRoomMembersRequest->SetVerb("GET");
	MatrixRoomMembersRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	MatrixRoomMembersRequest->ProcessRequest();
}

void MatrixNativeAPI::OnMatrixRoomMembersResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnMatrixRoomMembersResponseDelegate TheDelegate)
{
	MatrixRoomMembersResponse RoomMembersResponse;
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (JsonObject->HasField("chunk"))
			{
				EMatrixMembership Membership = ConvertStringToMatrixMembership(Request->GetURLParameter("membership"));

				FString RequestUrl = Request->GetURL();
				FString RoomID;
				FString Right, LeftTemp;
				RequestUrl.Split(TEXT("rooms/"), &LeftTemp, &Right);
				Right.Split(TEXT("/members"), &RoomID, &LeftTemp);

				TArray<FString> Users;
				//FString RoomID;
				for (TSharedPtr<FJsonValue> Member : JsonObject->GetArrayField("chunk"))
				{
					TSharedPtr<FJsonObject> MemberObject = Member->AsObject();
					if (MemberObject->HasField("state_key") && MemberObject->HasField("room_id"))
					{
						Users.Add(MemberObject->GetStringField("state_key"));
						//RoomID = MemberObject->GetStringField("room_id");
					}
				}

				RoomMembersResponse.Membership = Membership;
				RoomMembersResponse.UserIDs = Users;
				RoomMembersResponse.RoomID = RoomID;

				d_OnRoomMembersResponseReceived.ExecuteIfBound(EMatrixCoreError::Success, RoomMembersResponse, "");
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success, RoomMembersResponse);
			}
			else if (JsonObject->HasField("error") && JsonObject->HasField("errcode"))
			{
				d_OnRoomMembersResponseReceived.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")), RoomMembersResponse, JsonObject->GetStringField("error"));
				TheDelegate.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")), RoomMembersResponse);
			}
			else
			{
				d_OnRoomMembersResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, RoomMembersResponse, "");
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN, RoomMembersResponse);
			}
		}
		else
		{
			d_OnRoomMembersResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, RoomMembersResponse, "");
			TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN, RoomMembersResponse);
		}
	}
	else
	{
		d_OnRoomMembersResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, RoomMembersResponse, "");
		TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN, RoomMembersResponse);
	}
}


void MatrixNativeAPI::SendMessage(const FString& AccessToken, const FString& RoomID, const FString& Text, const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString SendMessageUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/send/m.room.message/" + GenerateTransactionID(MATRIX_TRANSATION_ID_LENGTH);

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);

	RequestObj->SetStringField("msgtype", "m.text");
	RequestObj->SetStringField("body", Text);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixSendMessageRequest = p_HttpModule->CreateRequest();
		MatrixSendMessageRequest->SetURL(SendMessageUrl);
		MatrixSendMessageRequest->OnProcessRequestComplete().BindLambda([TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				// TODO: check for the actual error broadcast that
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		});

		MatrixSendMessageRequest->SetVerb("PUT");
		MatrixSendMessageRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		MatrixSendMessageRequest->SetHeader("Content-Type", "application/json");
		MatrixSendMessageRequest->SetContentAsString(RequestBody);
		MatrixSendMessageRequest->ProcessRequest();
	}
}


void MatrixNativeAPI::SendCallInvite(const FString& AccessToken, const FString& RoomID, const FString& CallID, const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	// TODO: Rename and update the body functions
	FString SendCallInviteUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/send/m.call.invite/" + GenerateTransactionID(MATRIX_TRANSATION_ID_LENGTH);

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);

	RequestObj->SetStringField("call_id", CallID);
	RequestObj->SetNumberField("lifetime", MATRIX_CALL_INVITE_LIFETIME);
	RequestObj->SetNumberField("version", 1);

	TSharedPtr<FJsonObject> OfferObject = MakeShareable(new FJsonObject);
	OfferObject->SetStringField("type", "offer");
	OfferObject->SetStringField("sdp", "");

	RequestObj->SetObjectField("offer", OfferObject);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixSendCallInviteRequest = p_HttpModule->CreateRequest();
		MatrixSendCallInviteRequest->OnProcessRequestComplete().BindLambda([this, TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			UE_LOG(LogTemp, Warning, TEXT("On MatrixNativeAPI::SendCallInvite response - Response Code %d"), Response->GetResponseCode());
			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				// TODO: change all this to proper function and error call
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		});
		MatrixSendCallInviteRequest->SetURL(SendCallInviteUrl);
		MatrixSendCallInviteRequest->SetVerb("PUT");
		MatrixSendCallInviteRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		MatrixSendCallInviteRequest->SetHeader("Content-Type", "application/json");
		MatrixSendCallInviteRequest->SetContentAsString(RequestBody);
		MatrixSendCallInviteRequest->ProcessRequest();
	}
}

void MatrixNativeAPI::SendCallAnswer(const FString& AccessToken, const FString& RoomID, const FString& CallID, const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	// TODO: Rename and update the body functions
	FString SendAnswerUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/send/m.call.answer/" + GenerateTransactionID(MATRIX_TRANSATION_ID_LENGTH);

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);

	RequestObj->SetStringField("call_id", CallID);
	RequestObj->SetNumberField("version", 1);

	TSharedPtr<FJsonObject> AnswerObject = MakeShareable(new FJsonObject);
	AnswerObject->SetStringField("type", "answer");
	AnswerObject->SetStringField("sdp", "");

	RequestObj->SetObjectField("answer", AnswerObject);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixSendAnswerRequest = p_HttpModule->CreateRequest();
		MatrixSendAnswerRequest->OnProcessRequestComplete().BindLambda([this, TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			UE_LOG(LogTemp, Warning, TEXT("On MatrixNativeAPI::SendCallAnswer response - Response Code %d"), Response->GetResponseCode());

			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				// TODO: change all this to proper function and error call
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		});
		MatrixSendAnswerRequest->SetURL(SendAnswerUrl);
		MatrixSendAnswerRequest->SetVerb("PUT");
		MatrixSendAnswerRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		MatrixSendAnswerRequest->SetHeader("Content-Type", "application/json");
		MatrixSendAnswerRequest->SetContentAsString(RequestBody);
		MatrixSendAnswerRequest->ProcessRequest();
	}
}

void MatrixNativeAPI::SendCallHangup(const FString& AccessToken, const FString& RoomID, const FString& CallID, EMatrixCallHangUpReason Reason, const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	// TODO: Rename and update the body functions
	FString SendHangUpUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/send/m.call.hangup/" + GenerateTransactionID(MATRIX_TRANSATION_ID_LENGTH);

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);

	RequestObj->SetStringField("call_id", CallID);
	RequestObj->SetNumberField("version", 1);
	if (Reason != EMatrixCallHangUpReason::None)
	{
		RequestObj->SetStringField("reason", "invite_timeout");
	}
	
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixSendHangUpRequest = p_HttpModule->CreateRequest();
		MatrixSendHangUpRequest->OnProcessRequestComplete().BindLambda([this, TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			UE_LOG(LogTemp, Warning, TEXT("On MatrixNativeAPI::SendCallHangup response - Response Code %d"), Response->GetResponseCode());

			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				// TODO: change all this to proper function and error call
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		});
		MatrixSendHangUpRequest->SetURL(SendHangUpUrl);
		MatrixSendHangUpRequest->SetVerb("PUT");
		MatrixSendHangUpRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		MatrixSendHangUpRequest->SetHeader("Content-Type", "application/json");
		MatrixSendHangUpRequest->SetContentAsString(RequestBody);
		MatrixSendHangUpRequest->ProcessRequest();
	}
}

void MatrixNativeAPI::GetMessages(const FString& AccessToken, const FString& RoomID, const FString& FromToken, bool bIsBackward, const FString& ToToken, int limit)
{
	FString MessagesUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/messages?from=" + FromToken;

	if (bIsBackward)
	{
		// TODO:
		MessagesUrl += "&dir=b";
	}
	else
	{
		MessagesUrl += "&dir=f";
	}

	if (FromToken != "")
	{
		MessagesUrl += "&to=" + ToToken;
	}

	if (limit != 10)
	{
		MessagesUrl += "&limit=" + FString::FromInt(limit);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixMessagesRequest = p_HttpModule->CreateRequest();
	MatrixMessagesRequest->OnProcessRequestComplete().BindRaw(this, &MatrixNativeAPI::OnMatrixGetMessagesResponseReceived);
	MatrixMessagesRequest->SetURL(MessagesUrl);
	MatrixMessagesRequest->SetVerb("GET");
	MatrixMessagesRequest->SetHeader("Content-Type", "application/x-www-form-urlencoded");
	MatrixMessagesRequest->SetHeader("Authorization", "Bearer " + AccessToken);

	MatrixMessagesRequest->ProcessRequest();
}

void MatrixNativeAPI::OnMatrixGetMessagesResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	MatrixGetMessagesResponse MessagesResponse;
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (JsonObject->HasField("chunk"))
			{
				FString RequestUrl = Request->GetURL();
				FString RoomID;
				FString Right, LeftTemp;
				RequestUrl.Split(TEXT("rooms/"), &LeftTemp, &Right);
				Right.Split(TEXT("/messages"), &RoomID, &LeftTemp);

				MessagesResponse.RoomEvents = ProcessEventsForJoinedRoom(JsonObject->GetArrayField("chunk"));
				
				MessagesResponse.RoomID = RoomID;

				FString StartToken;
				FString EndToken;
				if (JsonObject->HasField("start"))
				{
					MessagesResponse.StartToken = JsonObject->GetStringField("start");
				}
				if (JsonObject->HasField("end"))
				{
					MessagesResponse.EndToken = JsonObject->GetStringField("end");
				}

				d_OnGetMessagesResponseReceived.ExecuteIfBound(EMatrixCoreError::Success, MessagesResponse, "");

			}
			else if (JsonObject->HasField("error") && JsonObject->HasField("errcode"))
			{
				d_OnGetMessagesResponseReceived.ExecuteIfBound(ConvertStringToMatrixCoreError(JsonObject->GetStringField("errcode")), MessagesResponse, JsonObject->GetStringField("error"));
			}
			else
			{
				d_OnGetMessagesResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, MessagesResponse, "");
			}
		}
		else
		{
			d_OnGetMessagesResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, MessagesResponse, "");
		}
	}
	else
	{
		d_OnGetMessagesResponseReceived.ExecuteIfBound(EMatrixCoreError::UNKNOWN, MessagesResponse, "");
	}
}

FMatrixJoinedRoomEvents MatrixNativeAPI::ProcessEventsForJoinedRoom(const TArray<TSharedPtr<FJsonValue>>& Events)
{
	FMatrixJoinedRoomEvents JoinedRoomEvents;

	TArray<FMatrixTextMessage> Messages;
	TArray<FMatrixCallEvent> CallInvites;
	TArray<FMatrixCallEvent> CallAnswers;
	TArray<FMatrixCallEvent> CallHangUps;
	TArray<TPair<EJoinedRoomEventType, int32>> EventOrder;

	for (auto Event : Events)
	{
		if (Event->AsObject()->HasField("type") && Event->AsObject()->HasField("content") && Event->AsObject()->HasField("sender")
			&& Event->AsObject()->HasField("origin_server_ts") && Event->AsObject()->HasField("event_id"))
		{

			int64 TimeInMS = FCString::Atoi64(*Event->AsObject()->GetStringField("origin_server_ts"));
			FDateTime TimeStamp = FDateTime::FromUnixTimestamp(TimeInMS / 1000) + FTimespan(ETimespan::TicksPerMillisecond * (TimeInMS % 1000));
			FString EventType = Event->AsObject()->GetStringField("type");

			if (EventType == "m.room.message")
			{
				TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
				if (ContentObject->HasField("msgtype") && ContentObject->GetStringField("msgtype") == "m.text" && ContentObject->HasField("body"))
				{
					FString Sender = Event->AsObject()->GetStringField("sender");

					FMatrixTextMessage TextMessage;
					TextMessage.Message = ContentObject->GetStringField("body");
					TextMessage.TimeStamp = TimeStamp;
					TextMessage.Sender = Sender;
					TextMessage.EventID = Event->AsObject()->GetStringField("event_id");

					Messages.Add(TextMessage);
					EventOrder.Add(TPair<EJoinedRoomEventType, int32>(EJoinedRoomEventType::Message, Messages.Num() - 1));


					/*UE_LOG(LogMatrixCore, Warning, TEXT("NativeAPI::OnSyncResponseReceived: Sender = %s,  time stamp = %d  message = %s original timestamp = %s --- %s -- %s"), *Sender, TimeInSec, *TextMessage.Message, *Event->AsObject()->GetStringField("origin_server_ts"),
						*FText::AsDateTime(TimeStamp, EDateTimeStyle::Short, EDateTimeStyle::Short).ToString(), *FText::AsDateTime(FDateTime::Now(), EDateTimeStyle::Short, EDateTimeStyle::Short, FText::GetInvariantTimeZone()).ToString());*/

				}
			}

			if (EventType == "m.call.invite")
			{
				// TODO: change it later for now if it is not initial sync then call related stuff will displayed

				TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
				if (ContentObject->HasField("call_id") && ContentObject->HasField("lifetime"))//&& ContentObject->HasField("offer"))
				{
					FDateTime CallRequestExpireTS = TimeStamp + FTimespan(ETimespan::TicksPerMillisecond * MATRIX_CALL_INVITE_LIFETIME);
					/*FDateTime TimeNow = FDateTime::UtcNow();
					if (TimeNow < CallRequestExpireTS)
					{*/
						FString Sender = Event->AsObject()->GetStringField("sender");

						FMatrixCallEvent CallInvite;
						CallInvite.CallID = ContentObject->GetStringField("call_id");
						CallInvite.ExpireTime = CallRequestExpireTS;
						CallInvite.TimeStamp = TimeStamp;
						CallInvite.Sender = Sender;
						CallInvite.EventID = Event->AsObject()->GetStringField("event_id");

						CallInvites.Add(CallInvite);
						EventOrder.Add(TPair<EJoinedRoomEventType, int32>(EJoinedRoomEventType::CallInvite, CallInvites.Num() - 1));
						/*UE_LOG(LogMatrixCore, Warning, TEXT("NativeAPI::OnSyncResponseReceived: -- CALL INVITE Sender = %s,  time stamp = %s expire in %s"), *Sender,
							*FText::AsDateTime(TimeStamp, EDateTimeStyle::Short, EDateTimeStyle::Short).ToString(), *FText::AsDateTime(CallRequestExpireTS, EDateTimeStyle::Short, EDateTimeStyle::Short).ToString());*/
					//}


				}
			}

			if (EventType == "m.call.answer" )//&& !bIsIntialSync)
			{
				// TODO: change it later for now if it is not initial sync then call related stuff will displayed

				TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
				if (ContentObject->HasField("call_id") && ContentObject->HasField("answer"))
				{
					FString Sender = Event->AsObject()->GetStringField("sender");

					FMatrixCallEvent CallAnswer;
					CallAnswer.CallID = ContentObject->GetStringField("call_id");
					CallAnswer.TimeStamp = TimeStamp;
					CallAnswer.Sender = Sender;
					CallAnswer.EventID = Event->AsObject()->GetStringField("event_id");

					CallAnswers.Add(CallAnswer);
					EventOrder.Add(TPair<EJoinedRoomEventType, int32>(EJoinedRoomEventType::CallAnswer, CallAnswers.Num() - 1));
				}
			}

			if (EventType == "m.call.hangup" )//&& !bIsIntialSync)
			{
				// TODO: change it later for now if it is not initial sync then call related stuff will displayed

				TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
				if (ContentObject->HasField("call_id"))//&& ContentObject->HasField("reason"))
				{
					FString Sender = Event->AsObject()->GetStringField("sender");

					FMatrixCallEvent CallHangUp;
					CallHangUp.CallID = ContentObject->GetStringField("call_id");
					CallHangUp.TimeStamp = TimeStamp;
					CallHangUp.Sender = Sender;
					CallHangUp.EventID = Event->AsObject()->GetStringField("event_id");

					CallHangUps.Add(CallHangUp);
					EventOrder.Add(TPair<EJoinedRoomEventType, int32>(EJoinedRoomEventType::CallHangup, CallHangUps.Num() - 1));
				}
			}

			if (EventType == "m.room.name")
			{
				TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
				if (ContentObject->HasField("name"))
				{
					JoinedRoomEvents.RoomName = ContentObject->GetStringField("name");
				}
			}

			if (EventType == "m.room.power_levels")
			{
				TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");
				if (ContentObject->HasField("users"))
				{
					TSharedPtr<FJsonObject> UsersObject = ContentObject->GetObjectField("users");

					for (auto PowerValues : UsersObject->Values)
					{
						FString UserID = PowerValues.Key;
						int PowerLevel = PowerValues.Value->AsNumber();
						if (PowerLevel == 100)
						{
							JoinedRoomEvents.Admins.Add(UserID);
						}
					}
				}
			}
			if (Event->AsObject()->GetStringField("type") == "m.room.member" && Event->AsObject()->HasField("Content") &&
				Event->AsObject()->HasField("state_key"))
			{
				FString UserID = Event->AsObject()->GetStringField("state_key");
				TSharedPtr<FJsonObject> ContentObject = Event->AsObject()->GetObjectField("content");

				if (ContentObject->HasField("membership"))
				{
					EMatrixMembership Membership = ConvertStringToMatrixMembership(ContentObject->GetStringField("membership"));
					JoinedRoomEvents.MemberStatus.Add(UserID, Membership);
				}
			}
		}
	}

	JoinedRoomEvents.Messages = Messages;
	JoinedRoomEvents.CallInvites = CallInvites;
	JoinedRoomEvents.CallAnswers = CallAnswers;
	JoinedRoomEvents.CallHangUps = CallHangUps;
	JoinedRoomEvents.EventOrder = EventOrder;

	return JoinedRoomEvents;
}


// Update block list
// update:
//{
//"ignored_users": {
//	"@nagesh:comms.tessercon.com": {},
//		"@chaitanya:comms.tessercon.com" : {}
//}
//}

void MatrixNativeAPI::UpdateIgnoreList(const FString& AccessToken, const TArray<FString>& BlockedUsers, const FString& UserID, const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString AccountDataUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/user/" + UserID + "/account_data/m.ignored_user_list";

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> IgnoredUserObject = MakeShareable(new FJsonObject);
	for (auto BlockedUser : BlockedUsers)
	{
		IgnoredUserObject->SetObjectField(BlockedUser, NULL); //StringField(Pair.Key, Pair.Value);
	}

	RequestObj->SetObjectField("ignored_users", IgnoredUserObject);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		// UE_LOG(LogMatrixCore, Warning, TEXT("MatrixNativeAPI::Login %s"), *RequestBody);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixAccountDataRequest = p_HttpModule->CreateRequest();
		MatrixAccountDataRequest->OnProcessRequestComplete().BindLambda([TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		});
		MatrixAccountDataRequest->SetURL(AccountDataUrl);
		MatrixAccountDataRequest->SetVerb("PUT");
		MatrixAccountDataRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		MatrixAccountDataRequest->SetHeader("Content-Type", "application/json");
		MatrixAccountDataRequest->SetContentAsString(RequestBody);
		MatrixAccountDataRequest->ProcessRequest();
	}
}

void MatrixNativeAPI::UpdateRoomName(const FString& AccessToken, const FString& RoomID, const FString& NewName, const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString RoomNameStateUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/state/m.room.name";

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
	RequestObj->SetStringField("name", NewName);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MatrixRoomNameStateRequest = p_HttpModule->CreateRequest();
		// TODO: can make this error handling more specific
		// make an new function ...
		MatrixRoomNameStateRequest->OnProcessRequestComplete().BindLambda([TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		});
		MatrixRoomNameStateRequest->SetURL(RoomNameStateUrl);
		MatrixRoomNameStateRequest->SetVerb("PUT");
		MatrixRoomNameStateRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		MatrixRoomNameStateRequest->SetHeader("Content-Type", "application/json");
		MatrixRoomNameStateRequest->SetContentAsString(RequestBody);
		MatrixRoomNameStateRequest->ProcessRequest();
	}
}

void MatrixNativeAPI::KickUserFromRoom(const FString& AccessToken, const FString& RoomID, const FString& UserID, const FString& Reason, const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString KickUserUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/kick";

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
	RequestObj->SetStringField("user_id", UserID);
	RequestObj->SetStringField("reason", Reason);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> KickUserRequest = p_HttpModule->CreateRequest();
		// TODO: can make this error handling more specific
		// make an new function ...
		KickUserRequest->OnProcessRequestComplete().BindLambda([TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		});
		KickUserRequest->SetURL(KickUserUrl);
		KickUserRequest->SetVerb("POST");
		KickUserRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		KickUserRequest->SetHeader("Content-Type", "application/json");
		KickUserRequest->SetContentAsString(RequestBody);
		KickUserRequest->ProcessRequest();
	}
}

void MatrixNativeAPI::SendTypingNotificationForRoom(const FString& AccessToken, const FString& RoomID, const FString& UserID, bool bIstyping, int32 Millisec,
	const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString TypingUserUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/typing/" + UserID;

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
	RequestObj->SetBoolField("typing", bIstyping);
	RequestObj->SetNumberField("timeout", Millisec);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> TypingNotificationRequest = p_HttpModule->CreateRequest();
		// TODO: can make this error handling more specific
		// make an new function ...
		TypingNotificationRequest->OnProcessRequestComplete().BindLambda([TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		});
		TypingNotificationRequest->SetURL(TypingUserUrl);
		TypingNotificationRequest->SetVerb("PUT");
		TypingNotificationRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		TypingNotificationRequest->SetHeader("Content-Type", "application/json");
		TypingNotificationRequest->SetContentAsString(RequestBody);
		TypingNotificationRequest->ProcessRequest();
	}
}


void MatrixNativeAPI::UpdateFullyReadMarkerForRoom(const FString& AccessToken, const FString& RoomID, const FString& FullyReadEventID, const FString& ReadEventID,
	const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString FullReadMarkerUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/read_markers ";

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
	RequestObj->SetStringField("m.fully_read", FullyReadEventID);
	if (ReadEventID != "")
	{
		RequestObj->SetStringField("m.read", ReadEventID);
	}

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> FullReadMarkerUpdateRequest = p_HttpModule->CreateRequest();
		// TODO: can make this error handling more specific
		// make an new function ...
		FullReadMarkerUpdateRequest->OnProcessRequestComplete().BindLambda([TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}
		});
		FullReadMarkerUpdateRequest->SetURL(FullReadMarkerUrl);
		FullReadMarkerUpdateRequest->SetVerb("POST");
		FullReadMarkerUpdateRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		FullReadMarkerUpdateRequest->SetHeader("Content-Type", "application/json");
		FullReadMarkerUpdateRequest->SetContentAsString(RequestBody);
		FullReadMarkerUpdateRequest->ProcessRequest();
	}
}


void MatrixNativeAPI::UpdateMembershipStateManuallyToLeave(const FString& AccessToken, const FString& RoomID, const FString& UserID,
	const FOnMatrixAPIResponseDelegate& TheDelegate)
{
	FString UpdateStateUrl = FString::Printf(MATRIX_SERVER) + "/_matrix/client/r0/rooms/" + RoomID + "/state/m.room.member/" + UserID;

	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
	RequestObj->SetStringField("membership", "leave");

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UpdateMemberStateRequest = p_HttpModule->CreateRequest();
		// TODO: can make this error handling more specific
		// make an new function ...
		UpdateMemberStateRequest->OnProcessRequestComplete().BindLambda([this, TheDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (Response->GetResponseCode() == 200)
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::Success);
			}
			else
			{
				TheDelegate.ExecuteIfBound(EMatrixCoreError::UNKNOWN);
			}

		});

		UpdateMemberStateRequest->SetURL(UpdateStateUrl);
		UpdateMemberStateRequest->SetVerb("PUT");
		UpdateMemberStateRequest->SetHeader("Authorization", "Bearer " + AccessToken);
		UpdateMemberStateRequest->SetHeader("Content-Type", "application/json");
		UpdateMemberStateRequest->SetContentAsString(RequestBody);
		UpdateMemberStateRequest->ProcessRequest();
	}
}
