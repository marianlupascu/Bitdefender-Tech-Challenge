#include <Windows.h>
#include <cstdio>

char cmd[1000];

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

int main(int argc, char* argv[])
{
	//__asm {
	//	int 3
	//}

	TerminateProcessEx(atoi(argv[1]), 55);

	return 0;
}
