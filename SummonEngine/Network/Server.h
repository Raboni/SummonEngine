#pragma once
#include "NetManager.h"
#include <GrowingArray.hpp>
#include <Time.hpp>
#include <vector>
#include <string>

class Server : public NetManager
{
public:
	Server();
	virtual ~Server();

	void InitServer(const unsigned short aPort);
	void Update(const float aDeltaTime) override final;

protected:
	struct ClientAddress
	{
		std::string myName;
		float myDisconnectTimer;
		unsigned int myIP;
		unsigned short myPort;
		short myID;
	};

protected:
	template<class T>
	void SendCustomNetMessage(T& aMessage, const long long aClientIndex);
	template<class T>
	void SendGlobalCustomNetMessage(T& aMessage, const long long aExcludedClientIndex = -1);

	virtual void OnReceivedMessage(const NetMessage& aMessage);
	virtual void PostReceivedMessage(const NetMessage& aMessage);
	virtual void OnConnectedClient(const short aClientID);
	virtual void OnDisconnectedClient(const short aClientID);
	void ReceivedHandshake(NetHandshakeMessage& aMessage, const sockaddr_in& aSenderAddress) override;

	const long long FindConnectedClient(const short aClientID) const;
	const short PeekNextNewClientID() const;
	const short GetConnectedClientCount() const;
	const CU::GrowingArray<int>& GetConnectedClientIDList() const;

private:
	void PingClients();
	void ConnectClient(const sockaddr_in& aClientAddress, const std::string& aClientName, const short aClientID);
	void DisconnectClient(const size_t aClientIndex);

private:
	std::vector<ClientAddress> myConnectedClients;
	CU::GrowingArray<int> myConnectedClientIDs;
	CU::GrowingArray<int> myOpenClientIDs;
	float myPingTimer;
	short myNextClientID;
};

template<class T>
inline void Server::SendCustomNetMessage(T& aMessage, const long long aClientIndex)
{
	if (aClientIndex < 0)
	{
		return;
	}

	sockaddr_in sendAddress;
	sendAddress.sin_family = AF_INET;
	sendAddress.sin_addr.s_addr = myConnectedClients[aClientIndex].myIP;
	sendAddress.sin_port = myConnectedClients[aClientIndex].myPort;

	SendNetMessage(aMessage, sendAddress, myConnectedClients[aClientIndex].myID);
}

template<class T>
inline void Server::SendGlobalCustomNetMessage(T& aMessage, const long long aExcludedClientIndex)
{
	sockaddr_in sendAddress;
	sendAddress.sin_family = AF_INET;
	for (size_t clientIndex = 0; clientIndex < myConnectedClients.size(); clientIndex++)
	{
		if (clientIndex == static_cast<size_t>(aExcludedClientIndex))
		{
			continue;
		}

		sendAddress.sin_addr.s_addr = myConnectedClients[clientIndex].myIP;
		sendAddress.sin_port = myConnectedClients[clientIndex].myPort;

		SendNetMessage(aMessage, sendAddress, myConnectedClients[clientIndex].myID);
	}
}