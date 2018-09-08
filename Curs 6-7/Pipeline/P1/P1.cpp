#include <Windows.h>
#include <iostream>
#define TEXT 0
#define NR 1

typedef struct message {
	int type;
	int val;
	char text[100];
	};

message msg[10] = { {0, 0, "text"}, {1, 2000, ""}, {0, 0, "ceva"}, {1, 10, ""}, {1, 50, ""} };

int main()
{
	HANDLE hPipe = CreateNamedPipe(TEXT("\\.\\pipe\\P1P2"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	SetEnvironmentVariable(L"pipeName", L"\\.\\pipe\\P1P2");
	STARTUPINFOA si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	char ProcessName[100] = "P2";

	if (!CreateProcessA(nullptr, // No module name (use command line)
		ProcessName, // Command line
		nullptr, // Process handle not inheritable
		nullptr, // Thread handle not inheritable
		FALSE, // Set handle inheritance to FALSE
		0, // No creation flags
		nullptr, // Use parent's environment block
		nullptr, // Use parent's starting directory 
		&si, // Pointer to STARTUPINFO structure
		&pi) // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return 0;
	}
	ConnectNamedPipe(hPipe, FALSE);
	for(int index = 0; index <= 5; index++)
	{
		LPDWORD bytesWritten = nullptr;
		WriteFile(hPipe, &msg[index], sizeof(msg[index]), bytesWritten, NULL);
	}
	message mess;
	mess.type = 0;
	strcpy(mess.text, "STOP");
	LPDWORD bytesWritten = nullptr;
	WriteFile(hPipe, &mess, sizeof(mess), bytesWritten, NULL);
	WaitForSingleObject();

	return 0;
}
