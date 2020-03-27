#include "..\stdafx.h"
#include "NetMessage.h"

NetMessage::NetMessage()
{
	myGuaranteedID = 0;
	myTimeStamp = 0;
	mySenderID = 0;
	myTargetID = 0;
	myLastIndex = 0;
	myUnpackIndex = 0;
	myMessageType = 0;
	myIsGuaranteed = false;
	memset(myData, 0, NetMessageDataSize);
}
NetMessage::NetMessage(const char aType)
{
	myMessageType = aType;
	myGuaranteedID = 0;
	myTimeStamp = 0;
	mySenderID = 0;
	myTargetID = 0;
	myLastIndex = 0;
	myUnpackIndex = 0;
	myIsGuaranteed = false;
	memset(myData, 0, NetMessageDataSize);
}
NetMessage::NetMessage(const char* aData)
{
	myGuaranteedID = 0;
	myTimeStamp = 0;
	mySenderID = 0;
	myTargetID = 0;
	myLastIndex = 0;
	myUnpackIndex = 0;
	myMessageType = 0;
	myIsGuaranteed = false;
	memcpy(myData, aData, NetMessageDataSize);
}
NetMessage::NetMessage(const NetMessage& aMessage)
{
	myGuaranteedID = aMessage.myGuaranteedID;
	myTimeStamp = aMessage.myTimeStamp;
	mySenderID = aMessage.mySenderID;
	myTargetID = aMessage.myTargetID;
	myLastIndex = aMessage.myLastIndex;
	myUnpackIndex = aMessage.myUnpackIndex;
	myMessageType = aMessage.myMessageType;
	myIsGuaranteed = aMessage.myIsGuaranteed;
	memcpy(&myData, &aMessage.myData, NetMessageDataSize);
}
NetMessage::~NetMessage()
{
}
NetMessage & NetMessage::operator=(const NetMessage& aMessage)
{
	myGuaranteedID = aMessage.myGuaranteedID;
	myTimeStamp = aMessage.myTimeStamp;
	mySenderID = aMessage.mySenderID;
	myTargetID = aMessage.myTargetID;
	myLastIndex = aMessage.myLastIndex;
	myUnpackIndex = aMessage.myUnpackIndex;
	myMessageType = aMessage.myMessageType;
	myIsGuaranteed = aMessage.myIsGuaranteed;
	memcpy(&myData, &aMessage.myData, NetMessageDataSize);
	return *this;
}

void NetMessage::Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID)
{
	myLastIndex = 0;
	SetNetData(aTimeStamp, aSenderID, aTargetID);
	PackData(myMessageType);
	PackData(myLastIndex);
	PackData(mySenderID);
	PackData(myTargetID);
	PackData(myTimeStamp);
	PackData(myGuaranteedID);
	PackData(myIsGuaranteed);
	EndPack();
}
void NetMessage::Unpack()
{
	myUnpackIndex = 0;
	UnpackData(myMessageType);
	UnpackData(myLastIndex);
	UnpackData(mySenderID);
	UnpackData(myTargetID);
	UnpackData(myTimeStamp);
	UnpackData(myGuaranteedID);
	UnpackData(myIsGuaranteed);
	EndUnpack();
}

void NetMessage::Repack()
{
	Pack(myTimeStamp, mySenderID, myTargetID);
}

void NetMessage::SetGuaranteed(const bool aGuaranteed)
{
	myIsGuaranteed = aGuaranteed;
}

char* NetMessage::GetData()
{
	return myData;
}
const char* NetMessage::GetData() const
{
	return myData;
}
const char NetMessage::GetType() const
{
	return myMessageType;
}
const short NetMessage::GetSenderID() const
{
	return mySenderID;
}
const short NetMessage::GetTargetID() const
{
	return myTargetID;
}
const unsigned short NetMessage::GetSize() const
{
	return myLastIndex;
}
const bool NetMessage::IsGuaranteed() const
{
	return myIsGuaranteed;
}
const unsigned long long NetMessage::GetGuaranteedID() const
{
	return myGuaranteedID;
}

void NetMessage::SetNetData(const long long aTimeStamp, const short aSenderID, const short aTargetID)
{
	myTimeStamp = aTimeStamp;
	mySenderID = aSenderID;
	myTargetID = aTargetID;
}

bool NetMessage::PackData(const std::string& aData)
{
	if (myLastIndex + aData.length() + sizeof(int) > NetMessageDataSize)
	{
		return false;
	}
	Serialize(aData, myData, myLastIndex);
	return true;
}
void NetMessage::EndPack()
{
	unsigned short lastIndex = myLastIndex;
	myLastIndex = sizeof(char);
	PackData(lastIndex);
	myLastIndex = lastIndex;
}

bool NetMessage::UnpackData(std::string& aData)
{
	if (myUnpackIndex + sizeof(int) > myLastIndex)
	{
		return false;
	}
	unsigned short prevGetIndex = myUnpackIndex;
	Deserialize(aData, myData, myUnpackIndex);
	if (myUnpackIndex > myLastIndex)
	{
		myUnpackIndex = prevGetIndex;
		aData.clear();
		return false;
	}
	if (myUnpackIndex == myLastIndex)
	{
		myUnpackIndex = 0;
	}
	return true;
}
void NetMessage::EndUnpack()
{
}