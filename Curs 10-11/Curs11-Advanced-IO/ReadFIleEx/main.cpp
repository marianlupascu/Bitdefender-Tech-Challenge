#include <Windows.h>
#include <stdio.h>

#define BUFFER_SIZE 100*1024*1024

HANDLE hEvent = NULL;
char buffer[BUFFER_SIZE];
DWORD uRead = 0;

VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD  dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	printf("bytes read %u", dwNumberOfBytesTransfered);
	SetEvent(hEvent);
	return;
}

int main()
{
	hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
	DWORD dwId = 0;


	HANDLE hFile = CreateFile(L"d:\\out.txt", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	OVERLAPPED ov;
	memset(&ov, 0, sizeof(OVERLAPPED));
	//ov.hEvent = hEvent;

	BOOL bRead = ReadFileEx(hFile, buffer, BUFFER_SIZE, &ov, FileIOCompletionRoutine);

	DWORD dwResult = WaitForSingleObjectEx(hEvent, INFINITE, TRUE);

	return 0;
}