#include <windows.h>
#include <stdio.h>

void wmain(int argc, wchar_t * argv[])
{
	HANDLE hFile;
	BYTE   buff[4096];

	// Open the existing file.

	hFile = CreateFile(argv[1],									// open file
		GENERIC_READ,											// open for reading
		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,		// do not share
		NULL,													// no security
		OPEN_EXISTING,											// existing file only
		FILE_ATTRIBUTE_NORMAL,									// normal file
		NULL);													// no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Could not open One.txt.");
		return;
	}

	LockFile(hFile, 0, 0, 4096, 0);
	UnlockFile(hFile, 0, 0, 4096, 0);


	CloseHandle(hFile);
}