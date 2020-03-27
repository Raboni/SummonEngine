#pragma once
#include "NetMessage.h"

template<char aType>
class NetIntMessage : public NetMessage
{
public:
	NetIntMessage();
	NetIntMessage(const int aInt);
	NetIntMessage(const char* aData);
	NetIntMessage(const NetMessage& aMessage);
	~NetIntMessage();

	void Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID) override;
	void Unpack() override;

	void SetInt(const int aInt);
	const int GetInt() const;

private:
	int myInt;
};

template<char aType>
NetIntMessage<aType>::NetIntMessage() : NetMessage(aType)
{
	myInt = -1;
}
template<char aType>
NetIntMessage<aType>::NetIntMessage(const int aInt) : NetMessage(aType)
{
	myInt = aInt;
}
template<char aType>
NetIntMessage<aType>::NetIntMessage(const char* aData) : NetMessage(aData)
{
	Unpack();
}
template<char aType>
NetIntMessage<aType>::NetIntMessage(const NetMessage& aMessage) : NetMessage(aMessage.GetData())
{
	Unpack();
}
template<char aType>
NetIntMessage<aType>::~NetIntMessage()
{
}

template<char aType>
void NetIntMessage<aType>::Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID)
{
	NetMessage::Pack(aTimeStamp, aSenderID, aTargetID);
	PackData(myInt);
	NetMessage::EndPack();
}
template<char aType>
void NetIntMessage<aType>::Unpack()
{
	NetMessage::Unpack();
	UnpackData(myInt);
	NetMessage::EndUnpack();
}

template<char aType>
void NetIntMessage<aType>::SetInt(const int aInt)
{
	myInt = aInt;
}
template<char aType>
const int NetIntMessage<aType>::GetInt() const
{
	return myInt;
}