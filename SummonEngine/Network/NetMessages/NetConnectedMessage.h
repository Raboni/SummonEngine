#pragma once
#include "NetMessage.h"

class NetConnectedMessage : public NetMessage
{
public:
	NetConnectedMessage();
	NetConnectedMessage(const int aClientID);
	NetConnectedMessage(const char* aData);
	NetConnectedMessage(const NetMessage& aMessage);
	~NetConnectedMessage();

	void Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID) override;
	void Unpack() override;

	void SetClientID(const int aClientID);
	const int GetClientID() const;

private:
	int myClientID;
};

