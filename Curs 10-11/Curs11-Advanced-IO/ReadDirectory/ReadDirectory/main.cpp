#include <windows.h>
#include <stdio.h>

char buffer[0x100000];
int redo = 1;
int test = 0;

wchar_t wstrNumeDir[] = L"d:\\Personal\\carti";
#define LOG_FILE L"d:\\fisier.txt"

VOID WINAPI changed(DWORD dwErrorCode,DWORD dwNumberOfBytesTransfered,LPOVERLAPPED lpOverlapped)
{
	test=1;
	return;
}
DWORD WINAPI mythr(LPVOID lpThreadParameter)
{
	DWORD bRet = 0;
	OVERLAPPED ovp;
	memset(&ovp, 0, sizeof(OVERLAPPED));

	HANDLE hLog = CreateFile(LOG_FILE, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		0, NULL);

	HANDLE h = CreateFile(wstrNumeDir, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);

	while(redo)
	{
		
		BOOL bResult = ReadDirectoryChangesW( h, buffer, 0x100000, TRUE, 
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_ATTRIBUTES |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_LAST_ACCESS |
			FILE_NOTIFY_CHANGE_CREATION | 
			FILE_NOTIFY_CHANGE_SECURITY,
			&bRet,
			&ovp,
			&changed);
		
		
		FILE_NOTIFY_INFORMATION * p = (FILE_NOTIFY_INFORMATION *)buffer;
		//SleepEx(INFINITE, TRUE);
		size_t size = p->FileNameLength + MAX_PATH + 16;
		wchar_t * str = (wchar_t *)malloc(sizeof(wchar_t)* (size));
		_snwprintf(str, size, L"action=%d nume=%s\\", p->Action, wstrNumeDir);
		int currentLength = wcslen(str);
		memcpy((char*)str + currentLength*sizeof(wchar_t), p->FileName, p->FileNameLength* sizeof(wchar_t));
		str[currentLength + p->FileNameLength] = 0;
		wcscat_s(str, size, L"\n");
		DWORD nWritten = 0;
		WriteFile(hLog, str, wcslen(str)* sizeof(wchar_t), &nWritten, NULL);
		free(str);
	}
	CloseHandle(h);
	CloseHandle(hLog);
	return 0;
}

int main()
{
	DWORD id = 0;

	DWORD dwDisposition = 0;
	HKEY hk = NULL;
	LONG status = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		NULL,
		0,
		KEY_ALL_ACCESS,
		NULL,
		&hk,
		&dwDisposition);
	RegSetValueEx(hk, L"MyFirstKey", 0, REG_SZ, (BYTE *)L"D:\\Cursuri\\SistemeOperare\\Curs11-Advanced-IO\\ReadDirectory\\x64\\Debug\\ReadDirectory.exe", (wcslen(L"D:\\Cursuri\\SistemeOperare\\Curs11-Advanced-IO\\ReadDirectory\\Debug\\ReadDirectory.exe") + 1)* sizeof(wchar_t));
	RegCloseKey(hk);

	HANDLE hthr = CreateThread(NULL, 0, &mythr, NULL, 0, &id);

	WaitForSingleObject(hthr, INFINITE);
	return 0;
}