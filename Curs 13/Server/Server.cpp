#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "1509"

SOCKET ClientSocket[2] = {INVALID_SOCKET};
HANDLE hThreads[2];
int vect[2] = { 0, 1 };

DWORD WINAPI Fct(LPVOID param)
{
	
	int order = *(int*)param;
	int iResult, iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	std::cout << order<<'\n';
	do
	{
		iResult = recv(ClientSocket[order], recvbuf, recvbuflen, 0);
		for (int i = 0; i < iResult; i++)
			std::cout << recvbuf[i];
		std::cout << '\n';Sleep(10000);
		if (iResult > 0)
		{
			std::cout << order << ":";
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			recvbuf[0] = 'A';
			iSendResult = send(ClientSocket[order], recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket[order]);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket[order]);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket[order], SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket[order]);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket[order]);
	WSACleanup();

	Sleep(1000000);
	return 0;
}

int __cdecl main()
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;


	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	std::cout << "Am initializat Winsock\n";

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	std::cout << "Am rezolvat adresa si portul serverului\n";

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	std::cout << "Am creat socketul\n";

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "Acum am facut bind\n";

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "Astept sa se conecteze un client\n";

	int clientNr = 0;

	// Accept a client socket
	while (true)
	{
		ClientSocket[clientNr] = accept(ListenSocket, NULL, NULL);
		if (ClientSocket[clientNr] == INVALID_SOCKET)
		{
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		hThreads[clientNr] = CreateThread(nullptr,
			0,
			Fct,
			&vect[clientNr],
			0,
			nullptr);
		clientNr++;
		std::cout << "S-a conectat un client cel cu nr " << clientNr << "\n";
	}

	// No longer need server socket
	closesocket(ListenSocket);

	// Receive until the peer shuts down the connection
	/*
	do
	{
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		for (int i = 0; i < iResult; i++)
			std::cout << recvbuf[i];
		std::cout << '\n';
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			recvbuf[0] = 'A';
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	}
	while (iResult > 0);
	*/
	/*
	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	*/

	Sleep(10000);

	return 0;
}
