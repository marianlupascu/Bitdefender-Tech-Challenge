#include <Windows.h>


int main()
{
	DWORD dwDisposition = 0;
	HKEY hk = NULL;
	LONG status = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\TechChallengeTest",
		0,
		NULL,
		0,
		KEY_ALL_ACCESS,
		NULL,
		&hk,
		&dwDisposition);
	RegSetValueEx(hk, L"MyFirstKey", 0, REG_SZ, (BYTE *)L"Valoare", (wcslen(L"Valoare") + 1)* sizeof(wchar_t));
	RegCloseKey(hk);
	return 0;
}