/*
by _sinn3r

This program mimics the famous heartbleed vulnerability.

To compile:
cl.exe /MT heartbleed_example.c

On the client side, send the packet in this format:
[Type][Payload Length][Payload]

where:
- Type is a char (1 byte). Type 0x01 gets you the heartbleed-like bug.
- Payload length is an unsigned short (2 bytes)
- Payload is an array of characters (size depends on your payload length)
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Ws2tcpip.h>
#include <Winsock2.h>
#include <stdio.h>
#define DEFAULT_RECV_BUFFER_LEN 512
#define PORT 4444
#define REQ_READ 0x01
#pragma comment(lib, "Ws2_32.lib")

SOCKET serverSocket;
struct addrinfo* addrResult;

struct addrinfo* InitWinsock() {
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult !=0 ) {
    return NULL;
  }

  struct addrinfo hints;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  struct addrinfo *addrResult = NULL;
  iResult = getaddrinfo(NULL, "4444", &hints, &addrResult);
  if (iResult != 0) {
    WSACleanup();
    return NULL;
  }

  return addrResult;
}

SOCKET CreateSocket(struct addrinfo* addrResult) {
  SOCKET s = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
  if (s == INVALID_SOCKET) {
    freeaddrinfo(addrResult);
    WSACleanup();
    return -1;
  }

  int iResult = bind(s, addrResult->ai_addr, (int) addrResult->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    freeaddrinfo(addrResult);
    closesocket(s);
    return -1;
  }

  return s;
}

int Listen() {
  printf("- Listening on port %d\n", PORT);
  int iResult = listen(serverSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    return -1;
  }

  return 1;
}

VOID ClientRequestHandler(SOCKET clientSocket, SOCKADDR_IN clientInfo, BYTE packet[]) {
  unsigned char type = *(unsigned char*)&packet[0];
  switch (type) {
    case REQ_READ:
      unsigned short packetLen = htons(*(unsigned short*)&packet[1]);
      packet += (sizeof(unsigned char) + sizeof(unsigned short));
      char* payload = (char*) malloc(packetLen+1);
      //printf("- Payload address: %p\n", payload);
      memcpy(payload, packet, packetLen);
      payload[packetLen] = '\0';
      send(clientSocket, payload, packetLen, MSG_DONTROUTE);
      printf("- Returned %d bytes\n", packetLen);
      free(payload);
      payload = NULL;
    break;
    default:
      send(clientSocket, "OK\n", 3, MSG_DONTROUTE);
    break;
  }
}

VOID ReceiveClientMessages(SOCKET clientSocket, SOCKADDR_IN clientInfo) {
  BYTE recvBuffer[DEFAULT_RECV_BUFFER_LEN+1];
  unsigned int recvBufferLen = DEFAULT_RECV_BUFFER_LEN;
  unsigned int bytesRead = 0;
  do {
    // recv() will complete when the input ends with \r\d
    bytesRead = recv(clientSocket, recvBuffer, recvBufferLen, MSG_PEEK);
    if (bytesRead > 0) {
      recvBuffer[bytesRead] = '\0';
      ClientRequestHandler(clientSocket, clientInfo, recvBuffer);
      bytesRead = 0;
    }
  } while (bytesRead > 0);
}

VOID AcceptConnection() {
  printf("- Ready to accept a connection\r\n");
  SOCKADDR_IN clientInfo;
  int clientInfoLen = sizeof(clientInfo);
  SOCKET clientSocket = accept(serverSocket, (SOCKADDR*) &clientInfo, &clientInfoLen);
  PCSTR ip = inet_ntoa(clientInfo.sin_addr);
  if (clientSocket == INVALID_SOCKET) {
    return;
  }

  printf("- Received a connection\n");
  ReceiveClientMessages(clientSocket, clientInfo);

  printf("- Shutting down connection\n");
  int iResult = shutdown(clientSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    closesocket(clientSocket);
  }
}

VOID StartVulnerableServer() {
  struct addrinfo* addrResult = InitWinsock();
  if (!addrResult) {
    printf("- Failed to init Winsock.\n");
    return;
  }

  serverSocket = CreateSocket(addrResult);
  int iResult = Listen();
  if (!iResult) {
    return;
  }

  while (TRUE) {
    AcceptConnection();
  }
}

int main(int argc, char** argv) {
  StartVulnerableServer();
  return 0;
}
