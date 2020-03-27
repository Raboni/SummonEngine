#pragma once
#include "NetManager.h"
#include <atomic>

class Client : public NetManager
{
public:
	Client();
	virtual ~Client();

	void ConnectToServer(const std::string& aServerAddress, const unsigned short aPort, const std::string& aClientName = std::string());
	void Update(const float aDeltaTime) override final;

protected:
	const bool CanReceiveMessage(const char aMessageType) override;
	virtual void OnConnected();

	template<class T>
	void SendCustomNetMessage(T& aMessage);
	virtual void OnReceivedMessage(const NetMessage& aMessage);
	virtual void PostReceivedMessage(const NetMessage& aMessage);

	void Disconnect();
	const bool IsConnected() const;
	const bool IsServerResponding() const;

private:
	WSADATA myWinSockData;
	sockaddr_in myAddress;
	sockaddr_in myServerAddress;
	std::string myName;

	float myRespondTimer;
	float myReconnectTimer;
	std::atomic<bool> myIsConnected;
	bool myIsServerResponding;
};

template<class T>
inline void Client::SendCustomNetMessage(T& aMessage)
{
	SendNetMessage(aMessage, myServerAddress, 0);
}