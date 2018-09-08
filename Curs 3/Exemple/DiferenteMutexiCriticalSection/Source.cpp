#include <Windows.h>
#include <stdio.h>

int a = 0;
//CRITICAL_SECTION csSection;
HANDLE hMutex = CreateMutex(NULL,
	FALSE,
	NULL);

DWORD WINAPI myFuntion(LPVOID param) {

	for (int i = 0; i < 1000000; i++) {
		//EnterCriticalSection(&csSection);
		WaitForSingleObject(hMutex, INFINITE);
		a++;
		//LeaveCriticalSection(&csSection);
		ReleaseMutex(&hMutex);
	}
	return 0;
}

int wmain()
{
	HANDLE hThreads[2];
	DWORD threadId[2];

	//InitializeCriticalSection(&csSection);

	for (int i = 0; i < 2; i++) {
		hThreads[i] = CreateThread(NULL,
			0,
			myFuntion,
			nullptr,
			0,
			&threadId[i]);
	}
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	printf("a = %d", a);
	getchar();
	//DeleteCriticalSection(&csSection);
	CloseHandle(&hMutex);
	return 0;
}

