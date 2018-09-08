#define WIN32_LEAN_AND_MEAN

#define MAX_CLIENTS 100

//command Server.exe

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "1509"

SOCKET ClientSocket[MAX_CLIENTS] = {INVALID_SOCKET};
HANDLE hThreads[MAX_CLIENTS];
int vect[MAX_CLIENTS];

int Enumerate(wchar_t wszFolderToEnumerate[], char* & returnStatement);

DWORD WINAPI Fct(LPVOID param)
{
	int order = *(int*)param;
	int iResult, iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	do
	{
		iResult = recv(ClientSocket[order], recvbuf, recvbuflen, 0);
		std::cout << '\n';
		std::cout << "Clientul " << order + 1 << " cere optiunea " << atoi(recvbuf) << '\n';
		std::cout << "Si cere informatii din directorul ";
		

		std::string s;
		for (int i = 2; i < iResult; i++)
		{
			std::cout << recvbuf[i];
			s.push_back(recvbuf[i]);
		}
		std::cout << '\n';
		std::wstring ws(s.begin(), s.end());
		wchar_t aux[512];
		for (int i = 0; i <= ws.length(); i++)
		{
			aux[i] = ws[i];
		}


		if (iResult > 0)
		{
			// Echo the buffer back to the sender

			char* sendBuf = nullptr;
			Enumerate(aux, sendBuf);
			/*
			for(int i = 0; i < strlen(sendBuf); i++)
				std::cout << sendBuf[i];
			*/
			iSendResult = send(ClientSocket[order], sendBuf, strlen(sendBuf), 0);
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
	}
	while (iResult > 0);

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
	/*
	wchar_t a[100] = L"C:\\Users\\Marian\\Documents\\Documente\\Bitdefender Tech Challenge";
	char* aux = nullptr;
	Enumerate(a, aux);
	for(int i = 0; i < strlen(aux); i++)
		std::cout << aux[i];
	std::cout  <<'\n';
	Sleep(100000);
	*/
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;

	for (int i = 0; i < MAX_CLIENTS; i++)
		vect[i] = i;


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
		Sleep(10000000);
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

	return 0;
}

int Enumerate(wchar_t wszFolderToEnumerate[], char* & returnStatement)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	wchar_t szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	std::wstring construction;


	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.

	length_of_arg = wcsnlen_s(wszFolderToEnumerate, MAX_PATH);

	if (length_of_arg > (MAX_PATH - 3))
	{
		//wprintf(L"Directory path is too long.\n");
		return (-1);
	}

	//wprintf(L"Target directory is %s\n", wszFolderToEnumerate);
	construction.append(L"Target directory is ");
	construction.append(wszFolderToEnumerate);
	construction.push_back('\n');


	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	wcscpy_s(szDir, MAX_PATH, wszFolderToEnumerate);
	wcscat_s(szDir, MAX_PATH, L"\\*");

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		//wprintf(L"FindFirstFile");
		construction.append(L"FindFirstFile");
		return dwError;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!wcscmp(ffd.cFileName, L".") ||
				!wcscmp(ffd.cFileName, L".."))
				continue;
			//wprintf(L"  %s   <DIR>\n", ffd.cFileName);
			construction.append(L"  ");
			construction.append(ffd.cFileName);
			construction.append(L"  <DIR>\n");
			/*
			//pentru recursivitate
			wchar_t aux[1000];
			wcscpy_s(aux, wszFolderToEnumerate);
			wcscat_s(aux, L"\\");
			wcscat_s(aux, ffd.cFileName);
			Enumerate(aux);
			*/
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			//wprintf(L"  %s   %ld bytes\n", ffd.cFileName, filesize.QuadPart);
			construction.append(L"  ");
			construction.append(ffd.cFileName);
			construction.append(L"  \n");
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		construction.append(L"FindFirstFile- something went wrong \n");
		//wprintf(L"FindFirstFile- something went wrong last error %u", dwError);
	}

	std::string s(construction.begin(), construction.end());
	//std::cout << s;
	returnStatement = new char[s.length() + 1];
	for (int i = 0; i <= s.length(); i++)
	{
		returnStatement[i] = s[i];
	}

	FindClose(hFind);
	return dwError;
}
