#include "..\stdafx.h"
#include "NetPingMessage.h"

NetPingMessage::NetPingMessage() : NetMessage(1)
{
	myIsReceived = false;
}
NetPingMessage::NetPingMessage(const char* aData) : NetMessage(aData)
{
	Unpack();
}
NetPingMessage::NetPingMessage(const NetMessage& aMessage) : NetMessage(aMessage.GetData())
{
	Unpack();
}
NetPingMessage::~NetPingMessage()
{
}

void NetPingMessage::Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID)
{
	NetMessage::Pack(aTimeStamp, aSenderID, aTargetID);
	PackData(myIsReceived);
	NetMessage::EndPack();
}
void NetPingMessage::Unpack()
{
	NetMessage::Unpack();
	UnpackData(myIsReceived);
	NetMessage::EndUnpack();
}

void NetPingMessage::Received()
{
	myIsReceived = true;
}
const bool NetPingMessage::IsReceived() const
{
	return myIsReceived;
}