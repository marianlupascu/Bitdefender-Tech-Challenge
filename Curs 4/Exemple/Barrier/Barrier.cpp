#include <Windows.h>
#include <stdio.h>
SYNCHRONIZATION_BARRIER sb;

DWORD WINAPI MyFunc(LPVOID param)
{
	int * p = (int *)param;

	printf("Sunt threadul %d si urmeaza sa fac sleep.\n", *p);
	Sleep((*p + 1) * 1000);
	printf("Sunt threadul %d si astept pe bariera.\n", *p);

	EnterSynchronizationBarrier(&sb, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY);
	printf("Sunt threadul %d si am trecut de bariera.\n", *p);
	return 0;
}

int main()
{
	int params[4];
	HANDLE hThreads[4];
	DWORD dwThrId[4];
	InitializeSynchronizationBarrier(&sb, 4, 0);
	for (int i = 0; i < 4; i++)
	{
		params[i] = i;
		hThreads[i] = CreateThread(NULL, 0, MyFunc, &params[i], 0, &dwThrId[i]);

	}
	WaitForMultipleObjects(4, hThreads, TRUE, INFINITE);

	DeleteSynchronizationBarrier(&sb);
	return 0;
}