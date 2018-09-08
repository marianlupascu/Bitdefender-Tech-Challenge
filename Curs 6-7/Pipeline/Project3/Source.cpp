#include <Windows.h>
#include <cstdio>
#include <string>
#include <iostream>


BOOL TerminateProcessEx(DWORD dwProcessId, UINT uExitCode)
{
	DWORD dwDesiredAccess = PROCESS_TERMINATE;
	BOOL  bInheritHandle = FALSE;
	HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	if (hProcess == NULL)
		return FALSE;

	BOOL result = TerminateProcess(hProcess, uExitCode);

	CloseHandle(hProcess);

	return result;
}

void afisare(std::string input)
{
	int prev = 0;
	for(int i = 0; i < input.length(); i++)
	{
		if(input[i] == '\0' && input[i+1] == '\0')
			break;
		if(input[i]=='\0')
		{
			std::string aux = input.substr(prev, i - prev);
			std::cout << aux <<'\n';
			prev = i + 1;
		}
	}
}

int main(int argc, char* argv[])
{
	LPWCH lpEnvString = GetEnvironmentStringsW();
	LPWSTR lpszVariable = (LPWSTR)lpEnvString;
	while (*lpszVariable)
	{
		wprintf(L"%s\n", lpszVariable);
		lpszVariable += wcslen(lpszVariable) + 1;
	}

	getchar();
	return 0;
}
