#include <Windows.h>
#include <stdio.h>

BOOL WINAPI HandlerRoutine(DWORD CtrlType)
{
	switch (CtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n");
		break;
	default:
		printf("Event %u\n", CtrlType);
		break;
	}

	return TRUE;
}

int main()
{
	SetConsoleCtrlHandler(HandlerRoutine, TRUE);
	while (1)
		Sleep(100);
	return 0;
}