#include <windows.h>
#include <stdio.h>

int wmain(int argc, wchar_t * argv[])
{
	WIN32_FIND_STREAM_DATA wfsd;
	HANDLE hStream = FindFirstStreamW(argv[1],
		FindStreamInfoStandard,
		&wfsd,
		0);

	DWORD dwLastError = 0;
	dwLastError = GetLastError();
	while (dwLastError != ERROR_HANDLE_EOF)
	{
		wprintf(L"stream %s has size %I64d\n", wfsd.cStreamName, wfsd.StreamSize.QuadPart);
		FindNextStreamW(hStream, &wfsd);
		dwLastError = GetLastError();
	}
	CloseHandle(hStream);
	Sleep(100000);
	return 0;

}