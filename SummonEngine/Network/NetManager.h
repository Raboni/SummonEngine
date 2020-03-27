#pragma once
#include "CommonNetworkIncludes.h"
#include "NetMessages\NetMessage.h"
#include "NetMessages\NetHandshakeMessage.h"
#include <Time.hpp>
#include <GrowingArray.hpp>

constexpr float ourRecordDuration = 0.5f;
constexpr float ourResendInterval = 0.05f;
constexpr bool ourUseTimeouts = false;

class NetManager
{
public:
	NetManager();
	virtual ~NetManager();

	virtual void Update(const float aDeltaTime);

protected:
	void Init(const unsigned short aPort);
	virtual const bool CanReceiveMessage(const char aMessageType);

	NetMessage ReceiveNetMessage(sockaddr_in& aOutAddress);
	template<class T>
	void SendNetMessage(T& aMessage, const sockaddr_in& aTargetAddress, const short aTargetID);

	virtual void ReceivedHandshake(NetHandshakeMessage& aMessage, const sockaddr_in& aSenderAddress);
	void SetClientID(const short aClientID);
	void SetSocketBlocking(const bool aBlock);

	const short GetClientID() const;
	const unsigned long long GetDataAmountSent() const;

private:
	struct GuaranteedRecordSent
	{
		GuaranteedRecordSent(){}
		GuaranteedRecordSent(const NetMessage& aMessage, const sockaddr_in& aTargetAddress)
		{
			myMessage = aMessage;
			myTargetAddress = aTargetAddress;
			myResendTimer = ourResendInterval;
			myRemoveTimer = ourRecordDuration;
		}
		NetMessage myMessage;
		sockaddr_in myTargetAddress;
		float myResendTimer = 0.0f;
		float myRemoveTimer = 0.0f;
	};
	struct GuaranteedRecordReceived
	{
		GuaranteedRecordReceived(){}
		GuaranteedRecordReceived(const unsigned long long aGuaranteedID, const short aClientID)
		{
			myGuaranteedID = aGuaranteedID;
			myClientID = aClientID;
			myRemoveTimer = ourRecordDuration;
		}
		unsigned long long myGuaranteedID = 0;
		float myRemoveTimer = 0.0f;
		short myClientID = -1;
	};

private:
	void SendPackedNetMessage(const NetMessage& aMessage, const sockaddr_in& aTargetAddress);

private:
	WSADATA myWinSockData;
	sockaddr_in myAddress;
	SOCKET mySocket;
	CU::GrowingArray<GuaranteedRecordReceived> myReceivedGuaranteedList;
	CU::GrowingArray<GuaranteedRecordSent> mySentGuaranteedList;
	unsigned long long myNextGuaranteedID;
	unsigned long long myDataAmountSentCurr;
	unsigned long long myDataAmountSent;
	unsigned long long myDataAmountReceivedCurr;
	unsigned long long myDataAmountReceived;
	float myPrintTimer;
	short myClientID;
};

template<class T>
inline void NetManager::SendNetMessage(T& aMessage, const sockaddr_in& aTargetAddress, const short aTargetID)
{
	static_assert(std::is_base_of<NetMessage, T>::value);

	if (aMessage.IsGuaranteed())
	{
		aMessage.myGuaranteedID = myNextGuaranteedID++/*myOpenGuaranteedIDs.GetLast()*/;
		//myOpenGuaranteedIDs.RemoveLast();
		aMessage.Pack(CU::Time::GetCurrentTimeStamp(), myClientID, aTargetID);
		mySentGuaranteedList.Add(GuaranteedRecordSent(aMessage, aTargetAddress));
		//std::cout << "Sending guaranteed message with type: " << static_cast<int>(aMessage.GetType()) << " to client: " << aMessage.GetTargetID() << std::endl;
	}
	else
	{
		aMessage.Pack(CU::Time::GetCurrentTimeStamp(), myClientID, aTargetID);
	}
	
	int error = sendto(mySocket, aMessage.GetData(), aMessage.GetSize(), 0, reinterpret_cast<const sockaddr*>(&aTargetAddress), sizeof(aTargetAddress));
	if (error >= 0)
	{
		myDataAmountSentCurr += aMessage.GetSize();
	}
}
