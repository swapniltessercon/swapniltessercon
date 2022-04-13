#include "TesserconCoreCommon.h"
#include "MatrixCommon.h"



VivoxChannelProperties GetChannelPropertiesForChannelKey(EVivoxChannelKey Key)
{
	VivoxChannelProperties ChannelProperties;
	switch (Key)
	{
		case EVivoxChannelKey::NearByChannel:
		{
			ChannelProperties.Type = ChannelType::Positional;
			ChannelProperties.bShouldTransmitOnJoin = true;
			ChannelProperties.bConnectAudio = true;
			ChannelProperties.bConnectText = true;

			return ChannelProperties;
		}
		case EVivoxChannelKey::VoiceCallChannel:
		{
			ChannelProperties.Type = ChannelType::NonPositional;
			ChannelProperties.bShouldTransmitOnJoin = true;
			ChannelProperties.bConnectAudio = true;
			ChannelProperties.bConnectText = false;
			return ChannelProperties;
		}
		case EVivoxChannelKey::GlobalTextOnlyChannel:
		{
			ChannelProperties.Type = ChannelType::NonPositional;
			ChannelProperties.bShouldTransmitOnJoin = true;
			ChannelProperties.bConnectAudio = false;
			ChannelProperties.bConnectText = true;
			return ChannelProperties;
		}
		default:
		{
			ChannelProperties.Type = ChannelType::NonPositional;
			ChannelProperties.bShouldTransmitOnJoin = false;
			ChannelProperties.bConnectAudio = false;
			ChannelProperties.bConnectText = false;
			return ChannelProperties;
		}
	}
}

FString GetUsernameFromMatrixUsername(const FString& MatrixUsername)
{
	//FString Username = "@tess_108:comms.tessercon.com";
	FString Right;
	FString Username;
	MatrixUsername.Split("@", NULL, &Right);
	Right.Split(":", &Username, NULL);

	return Username;
}

FString GetMatrixFullUserIDForUsername(const FString& Username)
{
	FString Domain = FString::Printf(MATRIX_SERVER);
	FString HomeServer;
	Domain.Split("//", NULL, &HomeServer);

	FString UserID = "@" + Username + ":" + HomeServer;

	return UserID;
}