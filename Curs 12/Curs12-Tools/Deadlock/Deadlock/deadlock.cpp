#include <Windows.h>

HANDLE hThr[2];
HANDLE hThrSem[2];
HANDLE hMutexes[2];

DWORD WINAPI ThreadFunc(LPVOID param)
{
	int i = *((int *)param);
	WaitForSingleObject(hMutexes[i], INFINITE);

	SignalObjectAndWait(hThrSem[1 - i], hThrSem[i], INFINITE, FALSE);

	WaitForSingleObject(hMutexes[1-i], INFINITE);

	return 0;
}

int main()
{
	int params[2];
	DWORD dwId = 0;

	//HANDLE h = CreateFile(L"d:\\out2.txt", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);

	for (int i = 0; i < 2; i++)
	{
		params[i] = i;
		hThrSem[i] = CreateSemaphore(NULL, 0, 1, NULL);
		hMutexes[i] = CreateMutex(NULL, FALSE, NULL);
	}

	for (int i = 0; i < 2; i++)
	{
		hThr[i] = CreateThread(NULL, 0, ThreadFunc, &params[i], 0, &dwId);
	}

	WaitForMultipleObjects(2, hThr, TRUE, INFINITE);
	return 0;
}