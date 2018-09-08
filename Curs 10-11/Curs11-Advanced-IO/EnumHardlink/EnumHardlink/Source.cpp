#include <Windows.h>


int main()
{
	wchar_t linkname[MAX_PATH];
	DWORD nr = MAX_PATH;
	HANDLE h = FindFirstFileNameW(L"C:\\Users\\Marian\\Documents\\Documente\\Bitdefender Tech Challenge\\Curs 11\\out.txt", 0,&nr, linkname);
	DWORD dwErr = GetLastError();
	do
	{
		FindNextFileNameW(h, &nr, linkname);
		dwErr = GetLastError();
	} while (dwErr == ERROR_MORE_DATA || !dwErr);

	FindClose(h);
	return 0;
}