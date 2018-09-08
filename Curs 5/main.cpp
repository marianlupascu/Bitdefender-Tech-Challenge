#include <Windows.h>
#include "../pipeline/pipeline.h"
#include <stdio.h>


HMODULE hDll = NULL;

INITPIPELINE pfnInitPipeline;
GENERATEELEMENT pfnGenerateElement;
UNINITPIPELINE pfnUninitPipeline;

HANDLE hSemProduse = NULL;

CRITICAL_SECTION csOrder;
int nPiulita = 0;
int nSurub = 0;
int nProdus = 0;
int nrErrors = 0;
int nTotal = 0;

DWORD dwPiulitaTid = 0;
DWORD dwSurubTid = 0;
DWORD dwProdusTid = 0;

void CbkSurub()
{
	int localPiulita=0, localSurub=0, localProdus=0, localError=0;

	if (dwSurubTid == 0)
		dwSurubTid = GetCurrentThreadId();
	else if (dwSurubTid != GetCurrentThreadId())
	{
		nrErrors++;
		printf("Surub called from multiple threads.\n");

	}

	EnterCriticalSection(&csOrder);
	nSurub++;
	localPiulita = nPiulita;
	localSurub = nSurub;
	localProdus = nProdus;
	
	if (localSurub != nProdus + 1)
		localError = 1;

	if (localPiulita != nProdus + 1 && localPiulita != nProdus)
		localError = 2;
	LeaveCriticalSection(&csOrder);

	if (localError)
	{
		nrErrors++;
		printf("Current state surub=%d piulita=%d produs=%d err=%d\n", localSurub, localPiulita, localProdus, localError);
	}
}

void CbkPiulita()
{
	int localPiulita = 0, localSurub = 0, localProdus = 0, localError = 0;
	if (dwPiulitaTid == 0)
		dwPiulitaTid = GetCurrentThreadId();
	else if (dwPiulitaTid != GetCurrentThreadId())
	{
		nrErrors++;
		printf("Piulita called from multiple threads.\n");

	}

	EnterCriticalSection(&csOrder);
	nPiulita++;
	localPiulita = nPiulita;
	localSurub = nSurub;
	localProdus = nProdus;

	if (localPiulita != nProdus + 1)
		localError = 3;

	if (localSurub != nProdus + 1 && localSurub != nProdus)
		localError = 4;
	LeaveCriticalSection(&csOrder);

	if (localError)
	{
		nrErrors++;
		printf("Current state surub=%d piulita=%d produs=%d err=%d\n", localSurub, localPiulita, localProdus, localError);
	}
}

void CbkProdus()
{
	int localPiulita = 0, localSurub = 0, localProdus = 0, localError = 0;

	if (dwProdusTid == 0)
		dwProdusTid = GetCurrentThreadId();
	else if (dwProdusTid != GetCurrentThreadId())
	{
		nrErrors++;
		printf("Produs called from multiple threads.\n");

	}
	
	EnterCriticalSection(&csOrder);
	nProdus++;
	localPiulita = nPiulita;
	localSurub = nSurub;
	localProdus = nProdus;

	if (localSurub != nProdus )
		localError = 5;

	if (localPiulita != nProdus )
		localError = 6;
	LeaveCriticalSection(&csOrder);

	if (localError)
	{
		nrErrors++;
		printf("Current state surub=%d piulita=%d produs=%d err=%d\n", localSurub, localPiulita, localProdus, localError);
	}
	ReleaseSemaphore(hSemProduse, 1, NULL);
}

#define FIRST_ROUND_ELEMENTS	3
#define SECOND_ROUND_ELEMENTS   5
#define THIRD_ROUND_ELEMENTS   8

int main()
{

	hSemProduse = CreateSemaphore(NULL, 0, 10000000, NULL);
	hDll = LoadLibrary(L"C:\\Users\\mmurgan\\Desktop\\Task_Ionut_Dragut\\pipeline.dll");
	
	if (!hDll)
	{
		printf("could not load pipeline.dll.\n");
		return -1;
	}

	pfnInitPipeline = (INITPIPELINE)GetProcAddress(hDll, "InitPipeline");
	if (!pfnInitPipeline)
	{
		printf("could not find InitPipeline.\n");
		return -1;
	}
	pfnGenerateElement = (GENERATEELEMENT)GetProcAddress(hDll, "GenerateElement");
	if (!pfnGenerateElement)
	{
		printf("could not find GenerateElement.\n");
		return -1;
	}
	pfnUninitPipeline = (UNINITPIPELINE)GetProcAddress(hDll, "UninitPipeline");
	if (!pfnUninitPipeline)
	{
		printf("could not find UninitPipeline.\n");
		return -1;
	}
	for (int k = 0; k < 3; k++)
	{

		nPiulita = 0;
		nSurub = 0;
		nProdus = 0;
		nrErrors = 0;
		nTotal = 0;

		dwPiulitaTid = 0;
		dwSurubTid = 0;
		dwProdusTid = 0;

		InitializeCriticalSection(&csOrder);
		InitParams pInit;
		pInit.pfnPiulita = CbkPiulita;
		pInit.pfnSurub = CbkSurub;
		pInit.pfnProdus = CbkProdus;

		pfnInitPipeline(&pInit);

		for (int i = 0; i < FIRST_ROUND_ELEMENTS; i++)
		{
			pfnGenerateElement(SURUB);
			pfnGenerateElement(PIULITA);
		}

		for (int i = 0; i < FIRST_ROUND_ELEMENTS; i++)
			WaitForSingleObject(hSemProduse, INFINITE);

		nTotal = FIRST_ROUND_ELEMENTS;
		if (nrErrors || (nSurub != nTotal || nPiulita != nTotal || nProdus != nTotal))
		{
			printf("First round of tests failed.\n");
			nrErrors++;
		}
		if (nrErrors == 0)
			printf("First round completed successfully piulite=%d suruburi=%d produse=%d\n", nPiulita, nSurub, nProdus);
		Sleep(1000);

		for (int i = 0; i < SECOND_ROUND_ELEMENTS; i++)
			pfnGenerateElement(SURUB);
		Sleep(1000);
		if (nProdus > FIRST_ROUND_ELEMENTS)
		{
			printf("Second round of tests failed.\n");
			nrErrors++;
		}
		for (int i = 0; i < SECOND_ROUND_ELEMENTS; i++)
			pfnGenerateElement(PIULITA);

		for (int i = 0; i < SECOND_ROUND_ELEMENTS; i++)
			WaitForSingleObject(hSemProduse, INFINITE);

		nTotal += SECOND_ROUND_ELEMENTS;
		if (nrErrors || (nSurub != nTotal || nPiulita != nTotal || nProdus != nTotal))
		{
			printf("Second round of tests failed.\n");
			nrErrors++;
		}
		if (nrErrors == 0)
			printf("Second round completed successfully piulite=%d suruburi=%d produse=%d\n", nPiulita, nSurub, nProdus);
		Sleep(5000);

		for (int i = 0; i < THIRD_ROUND_ELEMENTS; i++)
			pfnGenerateElement(PIULITA);
		Sleep(1000);
		if (nProdus > THIRD_ROUND_ELEMENTS)
		{
			printf("Third round of tests failed.\n");
			nrErrors++;
		}
		for (int i = 0; i < THIRD_ROUND_ELEMENTS; i++)
		{
			pfnGenerateElement(SURUB);
			WaitForSingleObject(hSemProduse, INFINITE);
			if (nProdus != (FIRST_ROUND_ELEMENTS + SECOND_ROUND_ELEMENTS + i + 1))
			{
				printf("Third round of tests failed.\n");
				nrErrors++;

			}
		}
		nTotal += THIRD_ROUND_ELEMENTS;
		if (nrErrors || (nSurub != nTotal || nPiulita != nTotal || nProdus != nTotal))
		{
			printf("Third round of tests failed.\n");
			nrErrors++;
		}
		if (nrErrors == 0)
			printf("Third round completed successfully piulite=%d suruburi=%d produse=%d\n", nPiulita, nSurub, nProdus);

		Sleep(5000);

		DeleteCriticalSection(&csOrder);
		if (nrErrors == 0)
		{
			if (dwPiulitaTid != 0 && dwSurubTid != 0 && dwProdusTid != 0 &&
				dwPiulitaTid != dwSurubTid && dwPiulitaTid != dwProdusTid &&
				dwSurubTid != dwProdusTid)
				printf("Test completed successfully piulite=%d suruburi=%d produse=%d\n", nPiulita, nSurub, nProdus);
			else
				printf("Test failed - at least 2 of piulita, surub, produs called from the same trhead.\n");
		}
		else
			printf("Test failed.\n");

		pfnUninitPipeline();
	}
	return 0;
}