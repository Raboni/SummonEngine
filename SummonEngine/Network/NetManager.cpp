#include "stdafx.h"
#include "NetManager.h"
#include "NetMessages\NetPingMessage.h"
#include "NetMessages\NetIntMessage.h"
#include <DebugTools.h>

NetManager::NetManager() : myReceivedGuaranteedList(64), mySentGuaranteedList(64)
{
	myNextGuaranteedID = 0;
	myDataAmountSentCurr = 0;
	myDataAmountSent = 0;
	myPrintTimer = 0.0f;
	myClientID = -1;
}
NetManager::~NetManager()
{
	closesocket(mySocket);
	WSACleanup();
}

void NetManager::Update(const float aDeltaTime)
{
	for (int recordIndex = 0; recordIndex < mySentGuaranteedList.Size(); recordIndex++)
	{
		if (ourUseTimeouts)
		{
			mySentGuaranteedList[recordIndex].myRemoveTimer -= aDeltaTime;
			if (mySentGuaranteedList[recordIndex].myRemoveTimer <= 0.0f)
			{
				mySentGuaranteedList.RemoveCyclicAtIndex(recordIndex--);
				continue;
			}
		}
		mySentGuaranteedList[recordIndex].myResendTimer -= aDeltaTime;
		if (mySentGuaranteedList[recordIndex].myResendTimer <= 0.0f)
		{
			mySentGuaranteedList[recordIndex].myResendTimer = ourResendInterval;
			SendPackedNetMessage(mySentGuaranteedList[recordIndex].myMessage, mySentGuaranteedList[recordIndex].myTargetAddress);
		}
	}
	if (ourUseTimeouts)
	{
		for (int recordIndex = 0; recordIndex < myReceivedGuaranteedList.Size(); recordIndex++)
		{
			myReceivedGuaranteedList[recordIndex].myRemoveTimer -= aDeltaTime;
			if (myReceivedGuaranteedList[recordIndex].myRemoveTimer <= 0.0f)
			{
				myReceivedGuaranteedList.RemoveCyclicAtIndex(recordIndex--);
			}
		}
	}

	myPrintTimer -= aDeltaTime;
	if (myPrintTimer <= 0.0f)
	{
		myDataAmountSent = myDataAmountSentCurr;
		myDataAmountSentCurr = 0;
		myDataAmountReceived = myDataAmountReceivedCurr;
		myDataAmountReceivedCurr = 0;
		myPrintTimer = 1.0f;
		CU::Log("Sent: " + std::to_string(myDataAmountSent) + " bytes/s");
		CU::Log("Received: " + std::to_string(myDataAmountReceived) + " bytes/s");
		//CU::Log("sent guaranteed amount: " + std::to_string(mySentGuaranteedList.Size()));
		//CU::Log("next guaranteed id: " + std::to_string(myNextGuaranteedID));
	}
}

void NetManager::Init(const unsigned short aPort)
{
	myNextGuaranteedID = 0;
	myDataAmountSentCurr = 0;
	myDataAmountSent = 0;
	myPrintTimer = 1.0f;
	myClientID = -1;

	//startup
	WSAStartup(MAKEWORD(2, 2), &myWinSockData);
	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mySocket == INVALID_SOCKET)
	{
		WSACleanup();
		CU::Log("Network manager failed to create socket", CU::LogType::Error);
		return;
	}
	const int socketBufferSize = 66560;
	setsockopt(mySocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&socketBufferSize), sizeof(socketBufferSize));
	setsockopt(mySocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&socketBufferSize), sizeof(socketBufferSize));

	//bind
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	myAddress.sin_port = htons(aPort);
	int error = bind(mySocket, reinterpret_cast<sockaddr*>(&myAddress), sizeof(myAddress));
	if (error < 0)
	{
		CU::Log("Network manager failed to bind with error: " + std::to_string(WSAGetLastError()), CU::LogType::Error);
		return;
	}
}

const bool NetManager::CanReceiveMessage(const char aMessageType)
{
	aMessageType;
	return true;
}

static int ourAddressSize = sizeof(sockaddr_in);
constexpr int ourMessageMinSize = sizeof(char) + sizeof(unsigned short) + sizeof(short) + sizeof(short) + sizeof(long long);
NetMessage NetManager::ReceiveNetMessage(sockaddr_in& aOutAddress)
{
	char receivedData[NetMessageDataSize];
	int receivedDataLength = recvfrom(mySocket, receivedData, 512, 0, reinterpret_cast<sockaddr*>(&aOutAddress), &ourAddressSize);
	NetMessage returnMessage;
	if (receivedDataLength < ourMessageMinSize || receivedDataLength > NetMessageDataSize)
	{
		return returnMessage;
	}
	returnMessage = NetMessage(receivedData);
	returnMessage.Unpack();
	myDataAmountReceivedCurr += returnMessage.GetSize();

	if (!CanReceiveMessage(returnMessage.GetType()))
	{
		return NetPingMessage();
	}

	if (returnMessage.GetType() == 2)
	{
		NetHandshakeMessage handshake(returnMessage);
		ReceivedHandshake(handshake, aOutAddress);
		returnMessage = handshake;
	}
	if (returnMessage.GetType() == 1)
	{
		NetPingMessage ping(returnMessage);
		if (!ping.IsReceived())
		{
			ping.Received();
			SendNetMessage(ping, aOutAddress, returnMessage.GetSenderID());
			ping.mySenderID = returnMessage.GetSenderID();
			ping.myTargetID = returnMessage.GetTargetID();
		}
		return ping;
	}
	else if (returnMessage.IsGuaranteed())
	{
		NetMessage confirmMessage(6);
		confirmMessage.myGuaranteedID = returnMessage.GetGuaranteedID();
		SendNetMessage(confirmMessage, aOutAddress, returnMessage.GetSenderID());

		for (int recordIndex = 0; recordIndex < myReceivedGuaranteedList.Size(); recordIndex++)
		{
			if (myReceivedGuaranteedList[recordIndex].myClientID == returnMessage.GetSenderID() &&
				myReceivedGuaranteedList[recordIndex].myGuaranteedID == returnMessage.GetGuaranteedID())
			{
				myReceivedGuaranteedList[recordIndex].myRemoveTimer = 0.5f;
				//std::cout << "Received old guaranteed message with type: " << static_cast<int>(returnMessage.GetType()) << " from client: " << returnMessage.GetSenderID() << std::endl;
				return NetPingMessage();//temp ping so receiver continues checking other messages
			}
		}
		//add received record and send confirm message
		myReceivedGuaranteedList.Add(GuaranteedRecordReceived(returnMessage.GetGuaranteedID(), returnMessage.GetSenderID()));
		//std::cout << "Received new guaranteed message with type: " << static_cast<int>(returnMessage.GetType()) << " from client: " << returnMessage.GetSenderID() << std::endl;
	}
	else if (returnMessage.GetType() == 6)
	{
		for (int recordIndex = 0; recordIndex < mySentGuaranteedList.Size(); recordIndex++)
		{
			if (mySentGuaranteedList[recordIndex].myMessage.GetGuaranteedID() == returnMessage.GetGuaranteedID())
			{
				mySentGuaranteedList.RemoveCyclicAtIndex(recordIndex);
				return NetPingMessage();
			}
		}
	}
	return returnMessage;
}

void NetManager::SendPackedNetMessage(const NetMessage& aMessage, const sockaddr_in& aTargetAddress)
{
	int error = sendto(mySocket, aMessage.GetData(), aMessage.GetSize(), 0, reinterpret_cast<const sockaddr*>(&aTargetAddress), sizeof(aTargetAddress));
	if (error >= 0)
	{
		myDataAmountSentCurr += aMessage.GetSize();
	}
}

void NetManager::ReceivedHandshake(NetHandshakeMessage& aMessage, const sockaddr_in& aSenderAddress)
{
	aMessage;
	aSenderAddress;
}

void NetManager::SetClientID(const short aClientID)
{
	myClientID = aClientID;
}

void NetManager::SetSocketBlocking(const bool aBlock)
{
	u_long nonblocking_enabled = static_cast<u_long>(!aBlock);
	ioctlsocket(mySocket, FIONBIO, &nonblocking_enabled);
}

const short NetManager::GetClientID() const
{
	return myClientID;
}

const unsigned long long NetManager::GetDataAmountSent() const
{
	return myDataAmountSent;
}