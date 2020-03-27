#include "stdafx.h"
#include "CommonNetworkFunctions.h"
#include <Time.hpp>
#include <GrowingArray.hpp>

struct GuaranteedRecord
{
	int myID = -1;
	float myTimer = 0.0f;
};

static int ourAddressSize = sizeof(sockaddr_in);
static CU::GrowingArray<GuaranteedRecord> ourReceivedGuaranteedIDs(16);

void SendData(SOCKET& aSocket, const sockaddr_in& aTargetAddress, char* aData, const int aDataSize)
{
	/*int error = */sendto(aSocket, aData, aDataSize, 0, reinterpret_cast<const sockaddr*>(&aTargetAddress), sizeof(aTargetAddress));
	/*if (error < 0)
	{
		PRINT_ERROR("Client failed to send data");
	}*/
}

void SendNetPingMessage(SOCKET& aSocket, const sockaddr_in& aTargetAddress, const short aSenderID, const short aTargetID)
{
	NetMessage msg(1);
	msg.Pack(CU::Time::GetCurrentTimeStamp(), aSenderID, aTargetID);
	SendData(aSocket, aTargetAddress, msg.GetData(), msg.GetSize());
}

void SendNetHandshakeMessage(SOCKET& aSocket, const sockaddr_in& aTargetAddress, const short aSenderID, const short aTargetID)
{
	NetHandshakeMessage msg;
	msg.Pack(CU::Time::GetCurrentTimeStamp(), aSenderID, aTargetID);
	SendData(aSocket, aTargetAddress, msg.GetData(), msg.GetSize());
}
void SendNetHandshakeMessage(SOCKET& aSocket, const sockaddr_in& aTargetAddress, const std::string& aClientName, const short aTargetID)
{
	NetHandshakeMessage msg(aClientName);
	msg.Pack(CU::Time::GetCurrentTimeStamp(), -1, aTargetID);
	SendData(aSocket, aTargetAddress, msg.GetData(), msg.GetSize());
}

void SendNetDisconnectMessage(SOCKET& aSocket, const sockaddr_in& aTargetAddress, const short aSenderID, const short aTargetID)
{
	NetMessage msg(3);
	msg.Pack(CU::Time::GetCurrentTimeStamp(), aSenderID, aTargetID);
	SendData(aSocket, aTargetAddress, msg.GetData(), msg.GetSize());
}

constexpr int ourMessageMinSize = sizeof(char) + sizeof(unsigned short) + sizeof(short) + sizeof(short) + sizeof(long long);
NetMessage ReceiveNetMessage(SOCKET& aSocket, sockaddr_in& aOutAddress)
{
	char receivedData[NetMessageDataSize];
	int receivedDataLength = recvfrom(aSocket, receivedData, 512, 0, reinterpret_cast<sockaddr*>(&aOutAddress), &ourAddressSize);
	NetMessage returnMessage;
	if (receivedDataLength < ourMessageMinSize || receivedDataLength > NetMessageDataSize)
	{
		return returnMessage;
	}
	returnMessage = NetMessage(receivedData);
	returnMessage.Unpack();
	if (returnMessage.IsGuaranteed())
	{
		//bool alreadyRecieved = false;
		for (int recordIndex = 0; recordIndex < ourReceivedGuaranteedIDs.Size(); recordIndex++)
		{
			if (ourReceivedGuaranteedIDs[recordIndex].myID == returnMessage.GetGuaranteedID())
			{
				//resend confirm message or let receiver know to resend confirm?
				return NetMessage(1);//temp ping so receiver continues checking other messages
			}
		}
		//add received record and send confirm message
	}
	return returnMessage;
}

void UpdateGuaranteedRecords(const float aDeltaTime)
{
	for (int recordIndex = 0; recordIndex < ourReceivedGuaranteedIDs.Size(); recordIndex++)
	{
		ourReceivedGuaranteedIDs[recordIndex].myTimer -= aDeltaTime;
		if (ourReceivedGuaranteedIDs[recordIndex].myTimer <= 0.0f)
		{
			ourReceivedGuaranteedIDs.RemoveCyclicAtIndex(recordIndex--);
		}
	}
}
