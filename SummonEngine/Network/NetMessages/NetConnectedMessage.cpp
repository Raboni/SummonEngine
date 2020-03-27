#include "..\stdafx.h"
#include "NetConnectedMessage.h"

NetConnectedMessage::NetConnectedMessage() : NetMessage(4)
{
	SetGuaranteed(true);
}
NetConnectedMessage::NetConnectedMessage(const int aClientID) : NetMessage(4)
{
	myClientID = aClientID;
	SetGuaranteed(true);
}
NetConnectedMessage::NetConnectedMessage(const char* aData) : NetMessage(aData)
{
	Unpack();
}
NetConnectedMessage::NetConnectedMessage(const NetMessage& aMessage) : NetMessage(aMessage.GetData())
{
	Unpack();
}
NetConnectedMessage::~NetConnectedMessage()
{
}

void NetConnectedMessage::Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID)
{
	NetMessage::Pack(aTimeStamp, aSenderID, aTargetID);
	PackData(myClientID);
	NetMessage::EndPack();
}
void NetConnectedMessage::Unpack()
{
	NetMessage::Unpack();
	UnpackData(myClientID);
	NetMessage::EndUnpack();
}

void NetConnectedMessage::SetClientID(const int aClientID)
{
	myClientID = aClientID;
}

const int NetConnectedMessage::GetClientID() const
{
	return myClientID;
}
