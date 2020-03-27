#include "stdafx.h"
#include "Client.h"
#include "NetMessages\NetPingMessage.h"
#include "NetMessages\NetHandshakeMessage.h"

#include <DebugTools.h>
#include <Windows.h>
#include <Lmcons.h>
#include <iostream>
#include <string>

constexpr float ourTimeUntilNotResponding = 0.5f;
constexpr float ourRespondPingInterval = 0.05f;
constexpr float ourReconnectInterval = 0.2f;

Client::Client()
{
	myRespondTimer = ourTimeUntilNotResponding;
	myReconnectTimer = ourReconnectInterval;
	myIsConnected = false;
	myIsServerResponding = false;

	//set windows username as default client name
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserNameA(username, &username_len);
	myName = username;

	Init(0);
}
Client::~Client()
{
	Disconnect();
}

void Client::ConnectToServer(const std::string& aServerAddress, const unsigned short aPort, const std::string& aClientName)
{
	myRespondTimer = ourTimeUntilNotResponding;
	myReconnectTimer = ourReconnectInterval;
	myIsConnected = false;
	myIsServerResponding = false;
	SetClientID(-1);
	SetSocketBlocking(false);

	if (aClientName.length() > 0)
	{
		myName = aClientName;
	}
	else
	{
		char username[UNLEN + 1];
		DWORD username_len = UNLEN + 1;
		GetUserNameA(username, &username_len);
		myName = username;
	}

	myServerAddress.sin_family = AF_INET;
	myServerAddress.sin_port = htons(aPort);
	const int error = InetPtonA(AF_INET, aServerAddress.c_str(), &myServerAddress.sin_addr.s_addr);
	if (error == 0)
	{
		//PRINT_ERROR("Invalid IP address");
		CU::Log("Invalid IP address", CU::LogType::Error);
		return;
	}
	
	NetHandshakeMessage msg(myName);
	SendCustomNetMessage(msg);
}

void Client::Update(const float aDeltaTime)
{
	NetManager::Update(aDeltaTime);
	
	sockaddr_in receivedAddress;
	NetMessage receivedMessage;
	while (myIsConnected)
	{
		receivedMessage = ReceiveNetMessage(receivedAddress);
		if (receivedMessage.GetType() == 0)
		{
			break;
		}
		if (receivedMessage.GetTargetID() != GetClientID())
		{
			continue;
		}

		if (myIsConnected)
		{
			OnReceivedMessage(receivedMessage);
		}
		switch (receivedMessage.GetType())
		{
		case 1:
		{
			if (ourUseTimeouts)
			{
				if (!myIsServerResponding)
				{
					CU::Log("Server is responding", CU::LogType::Log);
					myIsServerResponding = true;
				}
				myRespondTimer = ourTimeUntilNotResponding;
			}
			break;
		}
		case 3:
		{
			CU::Log("You got disconnected by the server", CU::LogType::Warning);
			myIsConnected = false;
			myIsServerResponding = false;
			SetClientID(-1);
			break;
		}
		default:
			break;
		}
		if (myIsConnected)
		{
			PostReceivedMessage(receivedMessage);
		}
	}

	if (!myIsConnected)
	{
		myReconnectTimer -= aDeltaTime;
		if (myReconnectTimer <= 0.0f)
		{
			myReconnectTimer = ourReconnectInterval;

			NetHandshakeMessage msg(myName);
			SendCustomNetMessage(msg);
		}

		receivedMessage = ReceiveNetMessage(receivedAddress);
		if (receivedMessage.GetType() == 2)
		{
			SetClientID(receivedMessage.GetTargetID());
			char serverAddress[INET6_ADDRSTRLEN];
			InetNtopA(myServerAddress.sin_family, &myServerAddress.sin_addr, serverAddress, INET6_ADDRSTRLEN);
			CU::Log("Connected to " + std::string(serverAddress) + " with ID " + std::to_string(GetClientID()));
			myIsConnected = true;
			myIsServerResponding = true;
			OnConnected();
		}
	}
	else
	{
		if (ourUseTimeouts)
		{
			myRespondTimer -= aDeltaTime;
			if (myRespondTimer <= 0.0f)
			{
				NetPingMessage ping;
				SendCustomNetMessage(ping);
				if (myIsServerResponding)
				{
					CU::Log("Server is not responding", CU::LogType::Warning);
					myIsServerResponding = false;
				}
				myRespondTimer = ourRespondPingInterval;
			}
		}
	}
}

const bool Client::CanReceiveMessage(const char aMessageType)
{
	return myIsConnected || aMessageType == 2;
}

void Client::OnConnected()
{
}

void Client::OnReceivedMessage(const NetMessage& aMessage)
{
	aMessage;
}
void Client::PostReceivedMessage(const NetMessage& aMessage)
{
	aMessage;
}

void Client::Disconnect()
{
	if (myIsConnected)
	{
		NetMessage msg(3);
		SendCustomNetMessage(msg);
	}
	myIsConnected = false;
	myServerAddress = sockaddr_in();
}

const bool Client::IsConnected() const
{
	return myIsConnected;
}
const bool Client::IsServerResponding() const
{
	return myIsServerResponding;
}