#include <Windows.h>
#include <stdio.h>

#define BUFFER_SIZE 90*1024*1024

HANDLE hEvent = NULL;
char buffer[BUFFER_SIZE];
DWORD uRead = 0;

DWORD WINAPI MyFunc(void * lpParam)
{
	printf("wait to read buffer %u\n", GetTickCount());
	WaitForSingleObject(hEvent, INFINITE);
	printf("buffer read %u bytes %u.\n", GetTickCount(), uRead);
	return 0;
}

int main()
{
	hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
	DWORD dwId = 0;
	HANDLE hThread = CreateThread(NULL, 0, MyFunc, 0, 0, &dwId);

	HANDLE hFile = CreateFile(L"d:\\out.txt", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);

	OVERLAPPED ov;
	memset(&ov, 0, sizeof(OVERLAPPED));
	ov.hEvent = hEvent;
	ReadFile(hFile, buffer, BUFFER_SIZE, &uRead, &ov);

	WaitForSingleObject(hThread, INFINITE);

	return 0;
}