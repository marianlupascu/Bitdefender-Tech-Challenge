#include  <Windows.h>
#include "../../../Curs 5/Pachet/pipeline.h"
#include <queue>
#include <iostream>

CALLBACK_SURUB pfnSurub = nullptr;
CALLBACK_PIULITA pfnPiulita = nullptr;
CALLBACK_PRODUS pfnProdus = nullptr;

HANDLE hSemSurub[2];
HANDLE hSemPiulita[2];
HANDLE hSemPlan, hSemSProdus, hSemPProdus;
CRITICAL_SECTION csSurub, csPiulita, csPlan;
int nrSuruburiEliberate, nrPiuliteEliberate;
int nrSuruburi, nrPiulite;
HANDLE hThreads[4];
DWORD dThreadId[4];
std::queue<ELEMENT> qPlan;

DWORD WINAPI planFunction(LPVOID param);
DWORD WINAPI piulitaFunction(LPVOID param);
DWORD WINAPI surubFunction(LPVOID param);
DWORD WINAPI produsFunction(LPVOID param);

int InitPipeline(InitParams* pInitParams)
{
	if (pInitParams)
	{
		pfnSurub = pInitParams->pfnSurub;
		pfnPiulita = pInitParams->pfnPiulita;
		pfnProdus = pInitParams->pfnProdus;
	}
	else
		return -1;

	hSemSurub[0] = CreateSemaphore(nullptr, 0, MAXINT, nullptr); //semafor pentru banda de suruburi
	hSemSurub[1] = CreateSemaphore(nullptr, 0, MAXINT, nullptr);
	//semafor pentru banda de productie referitor la suruburi
	hSemPiulita[0] = CreateSemaphore(nullptr, 0, MAXINT, nullptr); //analog
	hSemPiulita[1] = CreateSemaphore(nullptr, 0, MAXINT, nullptr); //analog
	hSemPlan = CreateSemaphore(nullptr, 0, MAXINT, nullptr);

	hSemSProdus = CreateSemaphore(nullptr, 0, MAXINT, nullptr);
	hSemPProdus = CreateSemaphore(nullptr, 0, MAXINT, nullptr);

	InitializeCriticalSection(&csSurub);
	InitializeCriticalSection(&csPiulita);
	InitializeCriticalSection(&csPlan);

	hThreads[0] = CreateThread(nullptr,
	                           0,
	                           planFunction,
	                           nullptr,
	                           0,
	                           &dThreadId[0]);

	hThreads[1] = CreateThread(nullptr,
	                           0,
	                           surubFunction,
	                           nullptr,
	                           0,
	                           &dThreadId[1]);

	hThreads[2] = CreateThread(nullptr,
	                           0,
	                           piulitaFunction,
	                           nullptr,
	                           0,
	                           &dThreadId[2]);

	hThreads[3] = CreateThread(nullptr,
	                           0,
	                           produsFunction,
	                           nullptr,
	                           0,
	                           &dThreadId[3]);

	WaitForMultipleObjects(4, hThreads, TRUE, INFINITE);

	return 0;
}

int UninitPipeline()
{
	DeleteCriticalSection(&csSurub);
	DeleteCriticalSection(&csPiulita);
	DeleteCriticalSection(&csPlan);
	return 0;
}

int GenerateElement(const ELEMENT elem)
{
	if(elem == PIULITA)
	{
		std::cout << "Generez piulita\n";
	}
	else
	{
		std::cout << "Gednerez surub\n";
	}
	EnterCriticalSection(&csPlan);
	qPlan.push(elem);
	LeaveCriticalSection(&csPlan);
	ReleaseSemaphore(hSemPlan, 1, nullptr);
	return 0;
}


DWORD WINAPI planFunction(LPVOID param)
{
	WaitForSingleObject(hSemPlan, INFINITE);
	EnterCriticalSection(&csPlan);
	ELEMENT type = qPlan.front();
	qPlan.pop();
	LeaveCriticalSection(&csPlan);
	switch (type)
	{
		case PIULITA:
			EnterCriticalSection(&csPiulita);

			LeaveCriticalSection(&csPiulita);
			ReleaseSemaphore(hSemPiulita, 1, nullptr);
			break;
		case SURUB:
			EnterCriticalSection(&csSurub);

			LeaveCriticalSection(&csSurub);
			ReleaseSemaphore(hSemSurub, 1, nullptr);
			break;
	}
	return 0;
}

DWORD WINAPI piulitaFunction(LPVOID param)
{
	const DWORD dwResult = WaitForMultipleObjects(2, hSemPiulita, FALSE, INFINITE);
	if (dwResult == WAIT_OBJECT_0)
	{
		EnterCriticalSection(&csPiulita);
		if (nrPiuliteEliberate == 0)
		{
			nrPiuliteEliberate++;
			pfnPiulita();
		}
		else
		{
			nrPiulite++;
		}
		LeaveCriticalSection(&csPiulita);
	}
	else if (dwResult == WAIT_OBJECT_0 + 1)
	{
		EnterCriticalSection(&csPiulita);
		if (nrPiulite > 0)
		{
			nrPiulite--;
			nrPiuliteEliberate = 1;
			pfnPiulita();
		}
		LeaveCriticalSection(&csPiulita);
	}
	else return -1;
	return 0;
}

DWORD WINAPI surubFunction(LPVOID param)
{
	const DWORD dwResult = WaitForMultipleObjects(2, hSemSurub, FALSE, INFINITE);
	if (dwResult == WAIT_OBJECT_0)
	{
		EnterCriticalSection(&csSurub);
		if(nrSuruburiEliberate == 0)
		{
			nrSuruburiEliberate++;
			pfnSurub();
		}
		else
		{
			nrSuruburi++;
		}
		LeaveCriticalSection(&csSurub);
	}
	else if (dwResult == WAIT_OBJECT_0 + 1)
	{
		EnterCriticalSection(&csSurub);
		if(nrSuruburi > 0)
		{
			nrSuruburi--;
			nrSuruburiEliberate = 1;
			pfnSurub();
		}
		LeaveCriticalSection(&csSurub);
	}
	else return -1;
	return 0;
}

DWORD WINAPI produsFunction(LPVOID param)
{
	return 0;
}


