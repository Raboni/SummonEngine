#pragma once
#include "NetMessage.h"

class NetPingMessage : public NetMessage
{
public:
	NetPingMessage();
	NetPingMessage(const char* aData);
	NetPingMessage(const NetMessage& aMessage);
	~NetPingMessage();

	void Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID) override;
	void Unpack() override;

	void Received();
	const bool IsReceived() const;

private:
	bool myIsReceived;
};

