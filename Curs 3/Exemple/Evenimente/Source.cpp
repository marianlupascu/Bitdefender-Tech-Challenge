#include <Windows.h>
#include <stdio.h>

HANDLE event = NULL;

DWORD WINAPI myFuntion1(LPVOID param) {

	printf("Sunt in thread 1\n");
	Sleep(1000);
	printf("Am terminat 1\n");
	SetEvent(event);
	return 0;
}

DWORD WINAPI myFuntion2(LPVOID param) {

	printf("Sunt in thread 2\n");
	WaitForSingleObject(event, INFINITE);
	printf("Am primit semnal de la threadul no 1\n");
	printf("gata si 2");
	return 0;
}

int wmain()
{
	HANDLE hThreads[2];
	DWORD threadId[2];

	event = CreateEvent(NULL, FALSE, FALSE, FALSE);


	//InitializeCriticalSection(&csSection);

	hThreads[0] = CreateThread(NULL,
		0,
		myFuntion1,
		NULL,
		0,
		&threadId[0]);

	hThreads[1] = CreateThread(NULL,
		0,
		myFuntion2,
		NULL,
		0,
		&threadId[1]);

	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	getchar();
	return 0;
}

