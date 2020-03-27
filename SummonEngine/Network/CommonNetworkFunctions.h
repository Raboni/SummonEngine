#pragma once
#include "NetMessages\NetHandshakeMessage.h"
#include <winsock2.h>
#include <string>

void SendData(SOCKET& aSocket, const sockaddr_in& aTargetAddress, char* aData, const int aDataSize);
void SendNetPingMessage(SOCKET& aSocket, const sockaddr_in& aTargetAddress, const short aSenderID, const short aTargetID);
void SendNetHandshakeMessage(SOCKET& aSocket, const sockaddr_in& aTargetAddress, const short aSenderID, const short aTargetID);
void SendNetHandshakeMessage(SOCKET& aSocket, const sockaddr_in& aTargetAddress, const std::string& aClientName, const short aTargetID);
void SendNetDisconnectMessage(SOCKET& aSocket, const sockaddr_in& aTargetAddress, const short aSenderID, const short aTargetID);
NetMessage ReceiveNetMessage(SOCKET& aSocket, sockaddr_in& aOutAddress);
void UpdateGuaranteedRecords(const float aDeltaTime);