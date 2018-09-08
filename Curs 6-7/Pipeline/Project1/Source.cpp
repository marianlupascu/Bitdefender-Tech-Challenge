#include <Windows.h>
#include <cstdio>

char cmd[1000];

int main(int argc, char* argv[])
{
	//__asm {
	//	int 3
	//}

	STARTUPINFOA si = {0};
	PROCESS_INFORMATION pi = {0};
	if (argc != 2)
	{
		printf("Usage: %s [cmdline]\n", argv[0]);
		return 0;
	}

	for (int i = 1; i < argc; i++)
	{
		strcat(cmd, argv[i]);
		strcat(cmd, " ");
	}


	if (!CreateProcessA(nullptr, // No module name (use command line)
	                    cmd, // Command line
	                    nullptr, // Process handle not inheritable
	                    nullptr, // Thread handle not inheritable
	                    FALSE, // Set handle inheritance to FALSE
	                    0, // No creation flags
	                    nullptr, // Use parent's environment block
	                    nullptr, // Use parent's starting directory 
	                    &si, // Pointer to STARTUPINFO structure
	                    &pi) // Pointer to PROCESS_INFORMATION structure
	)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return 0;
	}

	return 0;
}
