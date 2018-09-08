#include <Windows.h>
#include <stdio.h>

int main()
{
	HANDLE hFile = CreateFile(L"c:\\Windows\\Prefetch\\CHROME.EXE-5FE9909D.pf",
		GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, 0, NULL);
	SetFilePointer(hFile, 0x64, NULL, FILE_BEGIN);
	DWORD offset = 0;
	DWORD length = 0;
	DWORD dwRead = 0;
	ReadFile(hFile, &offset, sizeof(DWORD), &dwRead, NULL);
	ReadFile(hFile, &length, sizeof(DWORD), &dwRead, NULL);
	SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
	wchar_t * buffer = (wchar_t *)malloc(length);
	ReadFile(hFile, buffer, length, &dwRead, NULL);
	wchar_t * p = buffer;
	while ((char *)p - (char *)buffer < length)
	{
		wprintf(L"%s\n", p);
		p += wcslen(p) + 1;
	}
	free(buffer);
	return 0;
}