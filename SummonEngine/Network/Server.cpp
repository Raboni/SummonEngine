#include "stdafx.h"
#include "Server.h"
#include "NetMessages\NetPingMessage.h"
#include "NetMessages\NetHandshakeMessage.h"
#include "NetMessages\NetConnectedMessage.h"
#include "NetMessages\NetDisconnectedMessage.h"
#include <DebugTools.h>
#include <thread>

constexpr float ourTimeUntilDisconnect = 0.5f;
constexpr float ourPingInterval = 0.05f;

Server::Server()
{
	myNextClientID = 1;
	myPingTimer = ourPingInterval;
}
Server::~Server()
{
}

void Server::InitServer(const unsigned short aPort)
{
	Init(aPort);

	SetClientID(0);
	myOpenClientIDs.Init(4);
	myConnectedClientIDs.Init(4);
	myNextClientID = 1;
	myPingTimer = ourPingInterval;
	SetSocketBlocking(false);
}

void Server::Update(const float aDeltaTime)
{
	NetManager::Update(aDeltaTime);

	sockaddr_in receiveAddress;
	NetMessage receivedMessage;
	while (true)
	{
		receivedMessage = ReceiveNetMessage(receiveAddress);
		if (receivedMessage.GetType() == 0)
		{
			break;
		}
		if (receivedMessage.GetTargetID() != 0)
		{
			continue;
		}

		OnReceivedMessage(receivedMessage);
		switch (receivedMessage.GetType())
		{
		case 1:
		{
			if (ourUseTimeouts)
			{
				long long senderClientIndex = FindConnectedClient(receivedMessage.GetSenderID());
				if (senderClientIndex != -1)
				{
					myConnectedClients[senderClientIndex].myDisconnectTimer = ourTimeUntilDisconnect;
				}
			}
			break;
		}
		case 2:
		{
			long long senderClientIndex = FindConnectedClient(receivedMessage.GetSenderID());
			if (senderClientIndex == -1)
			{
				/*bool foundClient = false;
				for (size_t clientIndex = 0; clientIndex < myConnectedClients.size(); clientIndex++)
				{
					if (myConnectedClients[clientIndex].myIP == receiveAddress.sin_addr.s_addr &&
						myConnectedClients[clientIndex].myPort == receiveAddress.sin_port)
					{
						foundClient = true;
						break;
					}
				}
				if (foundClient)
				{
					break;
				}*/
				NetHandshakeMessage handshake(receivedMessage);
				CU::Log(handshake.GetClientName() + " joined");
				ConnectClient(receiveAddress, handshake.GetClientName(), handshake.GetSenderID());
			}
			break;
		}
		case 3:
		{
			long long senderClientIndex = FindConnectedClient(receivedMessage.GetSenderID());
			if (senderClientIndex != -1)
			{
				CU::Log(myConnectedClients[senderClientIndex].myName + " left");
				DisconnectClient(static_cast<size_t>(senderClientIndex));
			}
			break;
		}
		case 0:
		default:
			break;
		}
		PostReceivedMessage(receivedMessage);
	}

	if (ourUseTimeouts)
	{
		myPingTimer -= aDeltaTime;
		if (myPingTimer <= 0.0f)
		{
			myPingTimer = ourPingInterval;
			PingClients();
		}
		for (size_t clientIndex = 0; clientIndex < myConnectedClients.size(); clientIndex++)
		{
			myConnectedClients[clientIndex].myDisconnectTimer -= aDeltaTime;
			if (myConnectedClients[clientIndex].myDisconnectTimer <= 0.0f)
			{
				CU::Log(myConnectedClients[clientIndex].myName + " took too long to respond and was disconnected", CU::LogType::Log);
				DisconnectClient(clientIndex);
			}
		}
	}
}

void Server::OnReceivedMessage(const NetMessage& aMessage)
{
	aMessage;
}
void Server::PostReceivedMessage(const NetMessage& aMessage)
{
	aMessage;
}

void Server::OnConnectedClient(const short aClientID)
{
	aClientID;
}
void Server::OnDisconnectedClient(const short aClientID)
{
	aClientID;
}

void Server::ReceivedHandshake(NetHandshakeMessage& aMessage, const sockaddr_in& aSenderAddress)
{
	if (aMessage.GetSenderID() == -1)
	{
		for (size_t clientIndex = 0; clientIndex < myConnectedClients.size(); clientIndex++)
		{
			if (myConnectedClients[clientIndex].myIP == aSenderAddress.sin_addr.s_addr &&
				myConnectedClients[clientIndex].myPort == aSenderAddress.sin_port)
			{
				aMessage.mySenderID = myConnectedClients[clientIndex].myID;
				aMessage.Repack();
				return;
			}
		}
		if (myOpenClientIDs.Size() > 0)
		{
			aMessage.mySenderID = static_cast<short>(myOpenClientIDs.GetLast());
			myOpenClientIDs.RemoveLast();
		}
		else
		{
			aMessage.mySenderID = myNextClientID++;
		}
		aMessage.Repack();
	}
}

const long long Server::FindConnectedClient(const short aClientID) const
{
	for (size_t clientIndex = 0; clientIndex < myConnectedClients.size(); clientIndex++)
	{
		if (myConnectedClients[clientIndex].myID == aClientID)
		{
			return static_cast<long long>(clientIndex);
		}
	}
	return -1;
}

const short Server::PeekNextNewClientID() const
{
	return myNextClientID;
}

const short Server::GetConnectedClientCount() const
{
	return static_cast<short>(myConnectedClients.size());
}

const CU::GrowingArray<int>& Server::GetConnectedClientIDList() const
{
	return myConnectedClientIDs;
}

void Server::PingClients()
{
	sockaddr_in sendAddress;
	sendAddress.sin_family = AF_INET;
	for (size_t clientIndex = 0; clientIndex < myConnectedClients.size(); clientIndex++)
	{
		sendAddress.sin_addr.s_addr = myConnectedClients[clientIndex].myIP;
		sendAddress.sin_port = myConnectedClients[clientIndex].myPort;
		
		NetPingMessage ping;
		SendCustomNetMessage(ping, clientIndex);
	}
}

void Server::ConnectClient(const sockaddr_in& aClientAddress, const std::string& aClientName, const short aClientID)
{
	ClientAddress client;
	client.myName = aClientName;
	client.myDisconnectTimer = ourTimeUntilDisconnect;
	client.myIP = aClientAddress.sin_addr.s_addr;
	client.myPort = aClientAddress.sin_port;
	client.myID = aClientID;
	myConnectedClients.push_back(client);
	myConnectedClientIDs.Add(client.myID);

	NetHandshakeMessage msg;
	SendCustomNetMessage(msg, static_cast<long long>(myConnectedClients.size()) - 1);

	//send already connected messages then send new connected message
	NetConnectedMessage clientConnectedMessage;
	for (size_t clientIndex = 0; clientIndex < myConnectedClients.size(); clientIndex++)
	{
		clientConnectedMessage.SetClientID(myConnectedClients[clientIndex].myID);
		SendCustomNetMessage(clientConnectedMessage, static_cast<long long>(myConnectedClients.size()) - 1);
	}
	clientConnectedMessage.SetClientID(client.myID);
	SendGlobalCustomNetMessage(clientConnectedMessage, static_cast<long long>(myConnectedClients.size()) - 1);

	OnConnectedClient(client.myID);
}

void Server::DisconnectClient(const size_t aClientIndex)
{
	if (aClientIndex < 0 || aClientIndex >= myConnectedClients.size())
	{
		return;
	}

	myConnectedClientIDs.RemoveCyclic(myConnectedClients[aClientIndex].myID);
	if (myConnectedClients.size() <= 1)
	{
		myNextClientID = 1;
		myOpenClientIDs.RemoveAll();
		myConnectedClientIDs.RemoveAll();
	}
	else if (myConnectedClients[aClientIndex].myID == myNextClientID - 1)
	{
		myNextClientID--;
		int removableOpenID = -1;
		while ((removableOpenID = myOpenClientIDs.Find(myNextClientID - 1)) != -1)
		{
			myOpenClientIDs.RemoveCyclicAtIndex(removableOpenID);
			myNextClientID--;
		}
	}
	else
	{
		myOpenClientIDs.Add(myConnectedClients[aClientIndex].myID);
	}

	sockaddr_in sendAddress;
	sendAddress.sin_family = AF_INET;
	sendAddress.sin_addr.s_addr = myConnectedClients[aClientIndex].myIP;
	sendAddress.sin_port = myConnectedClients[aClientIndex].myPort;
	
	NetMessage disconnectMessage(3);
	disconnectMessage.SetGuaranteed(true);
	SendCustomNetMessage(disconnectMessage, aClientIndex);

	NetDisconnectedMessage clientDisconnectedMessage(myConnectedClients[aClientIndex].myID);
	SendGlobalCustomNetMessage(clientDisconnectedMessage, aClientIndex);

	OnDisconnectedClient(myConnectedClients[aClientIndex].myID);

	myConnectedClients[aClientIndex] = myConnectedClients.back();
	myConnectedClients.pop_back();
}