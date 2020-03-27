#pragma once
#include "..\CommonNetworkIncludes.h"
#include <string>

constexpr unsigned short NetMessageDataSize = 512;

class NetMessage
{
public:
	NetMessage();
	NetMessage(const char aType);
	NetMessage(const char* aData);
	NetMessage(const NetMessage& aMessage);
	~NetMessage();
	NetMessage& operator=(const NetMessage& aMessage);

	virtual void Pack(const long long aTimeStamp, const short aSenderID, const short aTargetID);
	virtual void Unpack();
	void Repack();
	void SetGuaranteed(const bool aGuaranteed);
	char* GetData();
	const char* GetData() const;
	//0:none 1:ping 2:handshake 3:disconnect 4:connected 5:disconnected 6:confirm
	const char GetType() const;
	const short GetSenderID() const;
	const short GetTargetID() const;
	const unsigned short GetSize() const;
	const bool IsGuaranteed() const;
	const unsigned long long GetGuaranteedID() const;

protected:
	void SetNetData(const long long aTimeStamp, const short aSenderID, const short aTargetID);
	template<typename T>
	bool PackData(const T& aData);
	bool PackData(const std::string& aData);
	void EndPack();

	template<typename T>
	bool UnpackData(T& aData);
	bool UnpackData(std::string& aData);
	void EndUnpack();

private:
	friend class NetManager;
	friend class Server;

	unsigned long long myGuaranteedID;
	long long myTimeStamp;
	short mySenderID;
	short myTargetID;
	unsigned short myUnpackIndex;
	unsigned short myLastIndex;
	//0:none 1:ping 2:handshake 3:disconnect 4:connected 5:disconnected 6:confirm
	char myMessageType;
	char myData[NetMessageDataSize];
	bool myIsGuaranteed;
};

template<typename T>
inline bool NetMessage::PackData(const T& aData)
{
	if (myLastIndex + sizeof(T) > NetMessageDataSize)
	{
		return false;
	}
	Serialize(aData, myData, myLastIndex);
	return true;
}

template<typename T>
inline bool NetMessage::UnpackData(T& aData)
{
	bool hasUnpackedLastIndex = myUnpackIndex >= sizeof(char) + sizeof(unsigned short);
	if (hasUnpackedLastIndex && myUnpackIndex + sizeof(T) > myLastIndex)
	{
		return false;
	}
	Deserialize(aData, myData, myUnpackIndex);
	if (hasUnpackedLastIndex && myUnpackIndex >= myLastIndex)
	{
		myUnpackIndex = 0;
	}
	return true;
}
