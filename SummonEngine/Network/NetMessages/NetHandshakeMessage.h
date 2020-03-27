#pragma once
#include "NetMessage.h"
#include <string>

class NetHandshakeMessage : public NetMessage
{
public:
	NetHandshakeMessage();
	NetHandshakeMessage(const std::string& aClientName);
	NetHandshakeMessage(const char* aData);
	NetHandshakeMessage(const NetMessage& aMessage);
	~NetHandshakeMessage();

	void Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID) override;
	void Unpack() override;

	const std::string& GetClientName() const;

private:
	std::string myClientName;
};

