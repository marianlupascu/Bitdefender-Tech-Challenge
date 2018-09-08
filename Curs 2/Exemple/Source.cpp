#include <Windows.h>
#include <stdio.h>

void cat(wchar_t arg[])
{
	HANDLE hFile = CreateFile(arg,
	                          GENERIC_READ,
	                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
	                          nullptr,
	                          OPEN_EXISTING,
	                          0,
	                          nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dwLastError = GetLastError();
		printf("we have an error %lu", dwLastError);
		exit(1);
	}

	DWORD dwBytesRead = 0;
	char buffer[4096];

	BOOL bRead = ReadFile(hFile,
	                      buffer,
	                      4096,
	                      &dwBytesRead,
	                      nullptr);

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD dwBytesWritten = 0;

	WriteFile(hStdOut,
	          buffer,
	          dwBytesRead,
	          &dwBytesWritten,
	          nullptr);

	CloseHandle(hFile);
}


int wmain(int argc, wchar_t* argv[])
{

	//cat(argv[1]);

	HANDLE hFile = CreateFile(argv[1],
	                          GENERIC_READ,
	                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
	                          nullptr,
	                          OPEN_EXISTING,
	                          0,
	                          nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dwLastError = GetLastError();
		printf("we have an error %lu", dwLastError);
		exit(1);
	}

	DWORD dwBytesRead = 0;
	char buffer[4096];
	BOOL bRead;
	DWORD dwBytesWritten;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	do
	{
		bRead = ReadFile(hFile,
		                 buffer,
		                 4096,
		                 &dwBytesRead,
		                 NULL);

		dwBytesWritten = 0;

		WriteFile(hStdOut,
		          buffer,
		          dwBytesRead,
		          &dwBytesWritten,
		          NULL);
	}
	while (dwBytesRead && bRead);

	CloseHandle(hFile);

	getchar();

	return 0;
}
