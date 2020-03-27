#include "..\stdafx.h"
#include "NetDisconnectedMessage.h"

NetDisconnectedMessage::NetDisconnectedMessage() : NetMessage(5)
{
	SetGuaranteed(true);
}
NetDisconnectedMessage::NetDisconnectedMessage(const int aClientID) : NetMessage(5)
{
	myClientID = aClientID;
	SetGuaranteed(true);
}
NetDisconnectedMessage::NetDisconnectedMessage(const char* aData) : NetMessage(aData)
{
	Unpack();
}
NetDisconnectedMessage::NetDisconnectedMessage(const NetMessage& aMessage) : NetMessage(aMessage.GetData())
{
	Unpack();
}
NetDisconnectedMessage::~NetDisconnectedMessage()
{
}

void NetDisconnectedMessage::Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID)
{
	NetMessage::Pack(aTimeStamp, aSenderID, aTargetID);
	PackData(myClientID);
	NetMessage::EndPack();
}
void NetDisconnectedMessage::Unpack()
{
	NetMessage::Unpack();
	UnpackData(myClientID);
	NetMessage::EndUnpack();
}

void NetDisconnectedMessage::SetClientID(const int aClientID)
{
	myClientID = aClientID;
}

const int NetDisconnectedMessage::GetClientID() const
{
	return myClientID;
}