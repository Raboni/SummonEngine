#pragma once
#include "NetMessage.h"

class NetDisconnectedMessage : public NetMessage
{
public:
	NetDisconnectedMessage();
	NetDisconnectedMessage(const int aClientID);
	NetDisconnectedMessage(const char* aData);
	NetDisconnectedMessage(const NetMessage& aMessage);
	~NetDisconnectedMessage();

	void Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID) override;
	void Unpack() override;

	void SetClientID(const int aClientID);
	const int GetClientID() const;

private:
	int myClientID;
};

