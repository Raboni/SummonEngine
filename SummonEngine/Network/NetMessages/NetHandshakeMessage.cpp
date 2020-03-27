#include "..\stdafx.h"
#include "NetHandshakeMessage.h"

NetHandshakeMessage::NetHandshakeMessage() : NetMessage(2)
{
	SetGuaranteed(true);
}
NetHandshakeMessage::NetHandshakeMessage(const std::string& aClientName) : NetMessage(2)
{
	myClientName = aClientName;
	SetGuaranteed(true);
}
NetHandshakeMessage::NetHandshakeMessage(const char* aData) : NetMessage(aData)
{
	Unpack();
}
NetHandshakeMessage::NetHandshakeMessage(const NetMessage& aMessage) : NetMessage(aMessage.GetData())
{
	Unpack();
}
NetHandshakeMessage::~NetHandshakeMessage()
{
	myClientName.clear();
}

void NetHandshakeMessage::Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID)
{
	NetMessage::Pack(aTimeStamp, aSenderID, aTargetID);
	PackData(myClientName);
	NetMessage::EndPack();
}
void NetHandshakeMessage::Unpack()
{
	NetMessage::Unpack();
	UnpackData(myClientName);
	NetMessage::EndUnpack();
}

const std::string& NetHandshakeMessage::GetClientName() const
{
	return myClientName;
}
