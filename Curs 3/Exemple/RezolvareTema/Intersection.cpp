#include <Windows.h>
#include <stdio.h>
#include <queue>

#define NR_MASINI 10
HANDLE hEventNS;
HANDLE hEventEV;
HANDLE hEventMaster;
CRITICAL_SECTION csNS;
CRITICAL_SECTION csEV;

std::queue<DWORD> qMasiniNS;
std::queue<DWORD> qMasiniEV;

DWORD WINAPI FuncMaster(LPVOID lpParam)
{
	int ev = 0;//mai intai dam drumul EV;

	while (1)
	{
		ResetEvent(hEventMaster);
		if (ev == 0)//directie curenta NS
		{
			ResetEvent(hEventNS);
			Sleep(3000);
			ev = 1 - ev;
			printf("Master::Fac verde in directia EV.\n");
			SetEvent(hEventEV);
		}
		else
		{
			ResetEvent(hEventEV);
			Sleep(3000);
			ev = 1 - ev;
			printf("Master::Fac verde in directia NS.\n");
			SetEvent(hEventNS);
		}
		WaitForSingleObject(hEventMaster, 90000);
	}

	return 0;
}

DWORD WINAPI ThrMasina(LPVOID lpParam)
{
	srand(GetTickCount());
	int dir = rand() % 2;
	if (dir == 0) //ne deplasam NS
	{
		printf("ThId=%u ma deplasez NS.\n", GetCurrentThreadId());
		EnterCriticalSection(&csNS);
		if (qMasiniNS.empty())
		{
			DWORD dwResult = WaitForSingleObject(hEventNS, 0);
			if (dwResult == WAIT_OBJECT_0)
			{
				LeaveCriticalSection(&csNS);
				printf("ThId=%u NS am gasit semaforul verde, incerc sa trec.\n", GetCurrentThreadId());
				Sleep(3000);
				printf("ThId=%u NS am trecut.\n", GetCurrentThreadId());
			}
			else if (dwResult == WAIT_TIMEOUT)
			{
				qMasiniNS.push(GetCurrentThreadId());
				LeaveCriticalSection(&csNS);
				printf("ThId=%u NS am gasit semaforul rosu, astept.\n", GetCurrentThreadId());
				WaitForSingleObject(hEventNS, INFINITE);
				printf("ThId=%u NS s-a facut verde, incerc sa trec.\n", GetCurrentThreadId());
				Sleep(3000);
				EnterCriticalSection(&csNS);
				DWORD toBeRemoved = qMasiniNS.front();
				printf("removing %u\n", toBeRemoved);
				qMasiniNS.pop();
				LeaveCriticalSection(&csNS);
				printf("ThId=%u NS s-a facut verde, am trecut.\n", GetCurrentThreadId());
			}
		}
		else
		{
			int pozitie = qMasiniNS.size();
			qMasiniNS.push(GetCurrentThreadId());
			LeaveCriticalSection(&csNS);
			printf("ThId=%u NS am ajuns la coada, am de asteptat %d.\n", GetCurrentThreadId(), pozitie);
			while (pozitie)
			{
				DWORD dwResult = WaitForSingleObject(hEventNS, 0);
				if (dwResult == WAIT_OBJECT_0)
				{
					Sleep(1000);
					pozitie--;
				}
				else
				{
					WaitForSingleObject(hEventNS, INFINITE);
					Sleep(1000);
					pozitie--;
				}
			}
			printf("ThId=%u NS am ajuns la semafor, incerc sa trec.\n", GetCurrentThreadId());
			Sleep(3000);
			EnterCriticalSection(&csNS);
			DWORD toBeRemoved = qMasiniNS.front();
			printf("removing %u\n", toBeRemoved);
			qMasiniNS.pop();
			LeaveCriticalSection(&csNS);
			printf("ThId=%u NS Am trecut.\n", GetCurrentThreadId());

		}

	}
	else
	{
		printf("ThId=%u ma deplasez EV.\n", GetCurrentThreadId());
		EnterCriticalSection(&csEV);
		if (qMasiniEV.empty())
		{
			DWORD dwResult = WaitForSingleObject(hEventEV, 0);
			if (dwResult == WAIT_OBJECT_0)
			{
				LeaveCriticalSection(&csEV);
				printf("ThId=%u EV am gasit semaforul verde, incerc sa trec.\n", GetCurrentThreadId());
				Sleep(3000);
				printf("ThId=%u EV am trecut.\n", GetCurrentThreadId());
			}
			else if (dwResult == WAIT_TIMEOUT)
			{
				qMasiniEV.push(GetCurrentThreadId());
				LeaveCriticalSection(&csEV);
				printf("ThId=%u EV am gasit semaforul rosu, astept.\n", GetCurrentThreadId());
				WaitForSingleObject(hEventEV, INFINITE);
				printf("ThId=%u EV s-a facut verde, incerc sa trec.\n", GetCurrentThreadId());
				Sleep(3000);
				EnterCriticalSection(&csEV);
				DWORD toBeRemoved = qMasiniEV.front();
				printf("removing %u\n", toBeRemoved);
				qMasiniEV.pop();
				LeaveCriticalSection(&csEV);
				printf("ThId=%u EV s-a facut verde, am trecut.\n", GetCurrentThreadId());
			}
		}
		else
		{
			int pozitie = qMasiniEV.size();
			qMasiniEV.push(GetCurrentThreadId());
			LeaveCriticalSection(&csEV);
			printf("ThId=%u EV am ajuns la coada, am de asteptat %d.\n", GetCurrentThreadId(), pozitie);
			while (pozitie)
			{
				DWORD dwResult = WaitForSingleObject(hEventEV, 0);
				if (dwResult == WAIT_OBJECT_0)
				{
					Sleep(1000);
					pozitie--;
				}
				else
				{
					WaitForSingleObject(hEventEV, INFINITE);
					Sleep(1000);
					pozitie--;
				}
			}
			printf("ThId=%u EV am ajuns la semafor, incerc sa trec.\n", GetCurrentThreadId());
			Sleep(3000);
			EnterCriticalSection(&csEV);
			DWORD toBeRemoved = qMasiniEV.front();
			printf("removing %u\n", toBeRemoved);
			qMasiniEV.pop();
			LeaveCriticalSection(&csEV);
			printf("ThId=%u EV Am trecut.\n", GetCurrentThreadId());

		}

	}
	return 0;
}

int main()
{
	HANDLE * hMasini = NULL;
	DWORD * dwThreadIds = NULL;
	HANDLE hMaster;
	srand(GetTickCount());
	hMasini = (HANDLE *)malloc(NR_MASINI * sizeof(HANDLE));
	dwThreadIds = (DWORD *)malloc(NR_MASINI * sizeof(DWORD));

	InitializeCriticalSection(&csNS);
	InitializeCriticalSection(&csEV);

	hEventNS = CreateEvent(NULL, 1, 0, NULL);
	hEventEV = CreateEvent(NULL, 1, 0, NULL);
	hEventMaster = CreateEvent(NULL, 1, 0, NULL);
	DWORD dwThreadMaster = 0;
	hMaster = CreateThread(NULL, 0, FuncMaster, NULL, 0, &dwThreadMaster);

	for (int i = 0; i < NR_MASINI; i++)
	{
		hMasini[i] = CreateThread(NULL, 0, ThrMasina, NULL, 0, &dwThreadIds[i]);
		Sleep(10);
	}

	WaitForMultipleObjects(NR_MASINI, hMasini, TRUE, INFINITE);
	//for (int i = 0 ; i < NR_MASINI ; i++)
	// WaitForSingleObject(hMasini[i], INFINITE);
	WaitForSingleObject(hMaster, INFINITE);

	DeleteCriticalSection(&csNS);
	DeleteCriticalSection(&csEV);
	CloseHandle(hEventNS);
	CloseHandle(hEventEV);
	CloseHandle(hEventMaster);

	free(hMasini);
	free(dwThreadIds);
}