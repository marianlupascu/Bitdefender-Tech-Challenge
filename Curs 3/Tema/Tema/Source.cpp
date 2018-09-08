/* T
 * Simularea unei intersectii.
 *
 * Sa se scrie un program care genereaza N(sa zicem 200) threaduri,
 * fiecare thread avand o directie alesa aleator din una din valorile:
 * 0 - NS
 * 1 - SN
 * 2 - EV
 * 3 - VE
 * Pe fiecare directie NS respectiv EV se afla cate un semafor simulat de un eveniment.
 * Trebuie sa mai existe un thread master care modifica semafoarele, dand la fiecare
 * 90 de secunde drumul cate unui semafor si blocand pe celalalt, avand o perioada de
 * 3 secunde in care amandoua sunt rosii pentru a elibera intersectia.
 * Fiecare thread care ajunge in intersectie verifica daca mai sunt masini in fata lui
 * pe aceeasi directie si calculeaza timpul necesar pana sa treaca ca fiind numarul de
 * masini din fata lui + 3 secunde din momentul in care semaforul este verde, daca nu
 * este verde poate trece abia dupa ce masinile din fata lui au trecut + inca 3 secunde.
 */

#include <Windows.h>
#include <iostream>
#include <string>
#include <queue>
#include <vector>

const int numberOfThreads = 30;

HANDLE hEventNS = nullptr;
HANDLE hEventEV = nullptr;
HANDLE hEventMaster = nullptr;
CRITICAL_SECTION csPrint;
CRITICAL_SECTION csNS;
CRITICAL_SECTION csSN;
CRITICAL_SECTION csEV;
CRITICAL_SECTION csVE;
std::queue<DWORD> qNS;
std::queue<DWORD> qSN;
std::queue<DWORD> qEV;
std::queue<DWORD> qVE;
std::vector<int> ThrDir;

void initAll();
void freeAll();
void randomStuf();

DWORD WINAPI FctMaster(LPVOID param)
{
	bool direction = true;
	//false - NS/SN, true - EV/VE
	while (true)
	{
		ResetEvent(hEventMaster);
		if (direction)
		{
			ResetEvent(hEventEV);

			EnterCriticalSection(&csPrint);
			std::cout << "MASTER: Red on bouth directions\n";
			LeaveCriticalSection(&csPrint);

			Sleep(3000);
			direction = !direction;

			EnterCriticalSection(&csPrint);
			std::cout << "MASTER: Green on NS and Red on EV\n";
			LeaveCriticalSection(&csPrint);

			SetEvent(hEventNS);
		}
		else
		{
			ResetEvent(hEventNS);

			EnterCriticalSection(&csPrint);
			std::cout << "MASTER: Red on bouth directions\n";
			LeaveCriticalSection(&csPrint);

			Sleep(3000);
			direction = !direction;

			EnterCriticalSection(&csPrint);
			std::cout << "MASTER: Green on EV and Red on NS\n";
			LeaveCriticalSection(&csPrint);

			SetEvent(hEventEV);
		}
		WaitForSingleObject(hEventMaster, 12000);
	}
}

DWORD WINAPI FctMasina(LPVOID param)
{
	int direction = ThrDir[*static_cast<int*>(param)];

	EnterCriticalSection(&csPrint);
	std::cout << "CAR: " << GetCurrentThreadId() << " I have direction " << direction << '\n';
	LeaveCriticalSection(&csPrint);

	switch (direction)
	{
	case 0:
		if(qNS.empty())
		{
			DWORD dwResult = WaitForSingleObject(hEventNS, 0);
			if (dwResult == WAIT_OBJECT_0)
			{
				EnterCriticalSection(&csNS);
				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " NS am gasit semaforul verde, incerc sa trec.\n";
				LeaveCriticalSection(&csPrint);
				
				Sleep(3000);

				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " NS am trecut.\n";
				LeaveCriticalSection(&csPrint);
				LeaveCriticalSection(&csNS);
			}
			else 
				if (dwResult == WAIT_TIMEOUT)
				{
					EnterCriticalSection(&csNS);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " NS am gasit semaforul rosu, mai astept.\n";
					LeaveCriticalSection(&csPrint);

					qNS.push(GetCurrentThreadId());
					LeaveCriticalSection(&csNS);

					WaitForSingleObject(hEventNS, INFINITE);

					EnterCriticalSection(&csNS);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " NS s-a facut verde, incerc sa trec.\n";
					LeaveCriticalSection(&csPrint);

					Sleep(3000);

					DWORD toBeRemoved = qNS.front();
					qNS.pop();

					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " NS am trecut. (sters din coada)\n";
					LeaveCriticalSection(&csPrint);
					LeaveCriticalSection(&csNS);
				}
		}
		else
		{
			EnterCriticalSection(&csNS);
			EnterCriticalSection(&csPrint);
			std::cout << "CAR: " << GetCurrentThreadId() << " NS mai am de asteptat " << qNS.size() << " masini\n";
			LeaveCriticalSection(&csPrint);
			qNS.push(GetCurrentThreadId());
			while (qNS.front() != GetCurrentThreadId())
			{
				LeaveCriticalSection(&csNS);
				Sleep(1000);
				EnterCriticalSection(&csNS);
			}

			EnterCriticalSection(&csPrint);
			std::cout << "CAR: " << GetCurrentThreadId() << " Acum sunt primul la semafor\n";
			LeaveCriticalSection(&csPrint);
			LeaveCriticalSection(&csNS);

			DWORD dwResult = WaitForSingleObject(hEventNS, 0);
			if (dwResult == WAIT_OBJECT_0)
			{
				EnterCriticalSection(&csNS);
				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " NS am gasit semaforul verde, incerc sa trec.\n";
				LeaveCriticalSection(&csPrint);

				qNS.pop();
				Sleep(3000);

				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " NS am trecut.\n ";
				LeaveCriticalSection(&csPrint);
				LeaveCriticalSection(&csNS);
			}
			else
				if (dwResult == WAIT_TIMEOUT)
				{
					EnterCriticalSection(&csNS);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " NS am gasit semaforul rosu, mai astept.\n";
					LeaveCriticalSection(&csPrint);
					
					LeaveCriticalSection(&csNS);
					WaitForSingleObject(hEventNS, INFINITE);

					EnterCriticalSection(&csNS);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " NS s-a facut verde, incerc sa trec.\n";
					LeaveCriticalSection(&csPrint);

					Sleep(3000);

					qNS.pop();

					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " NS am trecut. (sters din coada)\n";
					LeaveCriticalSection(&csPrint);
					LeaveCriticalSection(&csNS);
				}
		}
		break;
	case 1:
		if (qSN.empty())
		{
			DWORD dwResult = WaitForSingleObject(hEventNS, 0);
			if (dwResult == WAIT_OBJECT_0)
			{
				EnterCriticalSection(&csSN);
				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " SN am gasit semaforul verde, incerc sa trec.\n";
				LeaveCriticalSection(&csPrint);

				Sleep(3000);

				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " SN am trecut.\n";
				LeaveCriticalSection(&csPrint);
				LeaveCriticalSection(&csSN);
			}
			else
				if (dwResult == WAIT_TIMEOUT)
				{
					EnterCriticalSection(&csSN);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " SN am gasit semaforul rosu, mai astept.\n";
					LeaveCriticalSection(&csPrint);

					qSN.push(GetCurrentThreadId());
					LeaveCriticalSection(&csSN);

					WaitForSingleObject(hEventNS, INFINITE);

					EnterCriticalSection(&csSN);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " SN s-a facut verde, incerc sa trec.\n";
					LeaveCriticalSection(&csPrint);

					Sleep(3000);

					DWORD toBeRemoved = qSN.front();
					qSN.pop();

					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " SN am trecut. (sters din coada)\n";
					LeaveCriticalSection(&csPrint);
					LeaveCriticalSection(&csSN);
				}
		}
		else
		{
			EnterCriticalSection(&csSN);
			EnterCriticalSection(&csPrint);
			std::cout << "CAR: " << GetCurrentThreadId() << " SN mai am de asteptat " << qSN.size() << " masini\n";
			LeaveCriticalSection(&csPrint);
			qSN.push(GetCurrentThreadId());
			while (qSN.front() != GetCurrentThreadId())
			{
				LeaveCriticalSection(&csSN);
				Sleep(1000);
				EnterCriticalSection(&csSN);
			}

			EnterCriticalSection(&csPrint);
			std::cout << "CAR: " << GetCurrentThreadId() << " Acum sunt primul la semafor\n";
			LeaveCriticalSection(&csPrint);
			LeaveCriticalSection(&csSN);

			DWORD dwResult = WaitForSingleObject(hEventNS, 0);
			if (dwResult == WAIT_OBJECT_0)
			{
				EnterCriticalSection(&csSN);
				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " SN am gasit semaforul verde, incerc sa trec.\n";
				LeaveCriticalSection(&csPrint);

				qSN.pop();
				Sleep(3000);

				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " SN am trecut.\n ";
				LeaveCriticalSection(&csPrint);
				LeaveCriticalSection(&csSN);
			}
			else
				if (dwResult == WAIT_TIMEOUT)
				{
					EnterCriticalSection(&csSN);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " SN am gasit semaforul rosu, mai astept.\n";
					LeaveCriticalSection(&csPrint);

					LeaveCriticalSection(&csSN);
					WaitForSingleObject(hEventNS, INFINITE);

					EnterCriticalSection(&csSN);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " SN s-a facut verde, incerc sa trec.\n";
					LeaveCriticalSection(&csPrint);

					Sleep(3000);

					qSN.pop();

					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " SN am trecut. (sters din coada)\n";
					LeaveCriticalSection(&csPrint);
					LeaveCriticalSection(&csSN);
				}
		}
		break;
	case 2:
		if (qEV.empty())
		{
			DWORD dwResult = WaitForSingleObject(hEventEV, 0);
			if (dwResult == WAIT_OBJECT_0)
			{
				EnterCriticalSection(&csEV);
				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " EV am gasit semaforul verde, incerc sa trec.\n";
				LeaveCriticalSection(&csPrint);

				Sleep(3000);

				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " EV am trecut.\n";
				LeaveCriticalSection(&csPrint);
				LeaveCriticalSection(&csEV);
			}
			else
				if (dwResult == WAIT_TIMEOUT)
				{
					EnterCriticalSection(&csEV);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " EV am gasit semaforul rosu, mai astept.\n";
					LeaveCriticalSection(&csPrint);

					qEV.push(GetCurrentThreadId());
					LeaveCriticalSection(&csEV);

					WaitForSingleObject(hEventEV, INFINITE);

					EnterCriticalSection(&csEV);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " EV s-a facut verde, incerc sa trec.\n";
					LeaveCriticalSection(&csPrint);

					Sleep(3000);

					DWORD toBeRemoved = qEV.front();
					qEV.pop();

					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " EV am trecut. (sters din coada)\n";
					LeaveCriticalSection(&csPrint);
					LeaveCriticalSection(&csEV);
				}
		}
		else
		{
			EnterCriticalSection(&csEV);
			EnterCriticalSection(&csPrint);
			std::cout << "CAR: " << GetCurrentThreadId() << " EV mai am de asteptat " << qEV.size() << " masini\n";
			LeaveCriticalSection(&csPrint);
			qEV.push(GetCurrentThreadId());
			while (qEV.front() != GetCurrentThreadId())
			{
				LeaveCriticalSection(&csEV);
				Sleep(1000);
				EnterCriticalSection(&csEV);
			}

			EnterCriticalSection(&csPrint);
			std::cout << "CAR: " << GetCurrentThreadId() << " Acum sunt primul la semafor\n";
			LeaveCriticalSection(&csPrint);
			LeaveCriticalSection(&csEV);

			DWORD dwResult = WaitForSingleObject(hEventEV, 0);
			if (dwResult == WAIT_OBJECT_0)
			{
				EnterCriticalSection(&csEV);
				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " EV am gasit semaforul verde, incerc sa trec.\n";
				LeaveCriticalSection(&csPrint);

				qEV.pop();
				Sleep(3000);

				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " EV am trecut.\n ";
				LeaveCriticalSection(&csPrint);
				LeaveCriticalSection(&csEV);
			}
			else
				if (dwResult == WAIT_TIMEOUT)
				{
					EnterCriticalSection(&csEV);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " EV am gasit semaforul rosu, mai astept.\n";
					LeaveCriticalSection(&csPrint);

					LeaveCriticalSection(&csEV);
					WaitForSingleObject(hEventEV, INFINITE);

					EnterCriticalSection(&csEV);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " EV s-a facut verde, incerc sa trec.\n";
					LeaveCriticalSection(&csPrint);

					Sleep(3000);

					qEV.pop();

					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " EV am trecut. (sters din coada)\n";
					LeaveCriticalSection(&csPrint);
					LeaveCriticalSection(&csEV);
				}
		}
		break;
	case 3:
		if (qVE.empty())
		{
			DWORD dwResult = WaitForSingleObject(hEventEV, 0);
			if (dwResult == WAIT_OBJECT_0)
			{
				EnterCriticalSection(&csVE);
				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " VE am gasit semaforul verde, incerc sa trec.\n";
				LeaveCriticalSection(&csPrint);

				Sleep(3000);

				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " VE am trecut.\n";
				LeaveCriticalSection(&csPrint);
				LeaveCriticalSection(&csVE);
			}
			else
				if (dwResult == WAIT_TIMEOUT)
				{
					EnterCriticalSection(&csVE);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " VE am gasit semaforul rosu, mai astept.\n";
					LeaveCriticalSection(&csPrint);

					qVE.push(GetCurrentThreadId());
					LeaveCriticalSection(&csVE);

					WaitForSingleObject(hEventEV, INFINITE);

					EnterCriticalSection(&csVE);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " VE s-a facut verde, incerc sa trec.\n";
					LeaveCriticalSection(&csPrint);
					
					Sleep(3000);

					DWORD toBeRemoved = qVE.front();
					qVE.pop();

					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " VE am trecut. (sters din coada)\n";
					LeaveCriticalSection(&csPrint);
					LeaveCriticalSection(&csVE);
				}
		}
		else
		{
			EnterCriticalSection(&csVE);
			EnterCriticalSection(&csPrint);
			std::cout << "CAR: " << GetCurrentThreadId() << " VE mai am de asteptat " << qEV.size() << " masini\n";
			LeaveCriticalSection(&csPrint);
			qVE.push(GetCurrentThreadId());
			while (qVE.front() != GetCurrentThreadId())
			{
				LeaveCriticalSection(&csVE);
				Sleep(1000);
				EnterCriticalSection(&csVE);
			}

			EnterCriticalSection(&csPrint);
			std::cout << "CAR: " << GetCurrentThreadId() << " Acum sunt primul la semafor\n";
			LeaveCriticalSection(&csPrint);
			LeaveCriticalSection(&csVE);

			DWORD dwResult = WaitForSingleObject(hEventEV, 0);
			if (dwResult == WAIT_OBJECT_0)
			{
				EnterCriticalSection(&csVE);
				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " VE am gasit semaforul verde, incerc sa trec.\n";
				LeaveCriticalSection(&csPrint);

				qVE.pop();
				Sleep(3000);

				EnterCriticalSection(&csPrint);
				std::cout << "CAR: " << GetCurrentThreadId() << " VE am trecut.\n ";
				LeaveCriticalSection(&csPrint);
				LeaveCriticalSection(&csVE);
			}
			else
				if (dwResult == WAIT_TIMEOUT)
				{
					EnterCriticalSection(&csVE);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " VE am gasit semaforul rosu, mai astept.\n";
					LeaveCriticalSection(&csPrint);

					LeaveCriticalSection(&csVE);
					WaitForSingleObject(hEventEV, INFINITE);

					EnterCriticalSection(&csVE);
					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " VE s-a facut verde, incerc sa trec.\n";
					LeaveCriticalSection(&csPrint);

					Sleep(3000);

					qVE.pop();

					EnterCriticalSection(&csPrint);
					std::cout << "CAR: " << GetCurrentThreadId() << " VE am trecut. (sters din coada)\n";
					LeaveCriticalSection(&csPrint);
					LeaveCriticalSection(&csVE);
				}
		}
		break;
	default: ;
	}


	return 0;
}

int wmain(const int argc, wchar_t* argv[])
{
	initAll();

	randomStuf();

	HANDLE hThreadMAster;
	HANDLE hThreads[numberOfThreads];
	DWORD dwThreadIdMaster;
	DWORD dwThreadId[numberOfThreads];
	int order[numberOfThreads];

	hThreadMAster = CreateThread(nullptr,
	                             0,
	                             FctMaster,
	                             nullptr,
	                             0,
	                             &dwThreadIdMaster);

	for (int i = 0; i < numberOfThreads; i++)
	{
		order[i] = i;
		hThreads[i] = CreateThread(nullptr,
		                           0,
		                           FctMasina,
		                           &order[i],
		                           0,
		                           &dwThreadId[i]);
	}

	WaitForMultipleObjects(numberOfThreads, hThreads, true, INFINITE);
	WaitForSingleObject(hThreadMAster, INFINITE);

	freeAll();

	while (true)
		Sleep(1000);

	return 0;
}

void initAll()
{
	InitializeCriticalSection(&csPrint);
	InitializeCriticalSection(&csNS);
	InitializeCriticalSection(&csEV);
	InitializeCriticalSection(&csSN);
	InitializeCriticalSection(&csVE);

	hEventNS = CreateEvent(nullptr, true, false, nullptr);
	hEventEV = CreateEvent(nullptr, true, false, nullptr);
	hEventMaster = CreateEvent(nullptr, true, false, nullptr);
}

void freeAll()
{
	DeleteCriticalSection(&csPrint);
	DeleteCriticalSection(&csNS);
	DeleteCriticalSection(&csEV);
	DeleteCriticalSection(&csSN);
	DeleteCriticalSection(&csVE);

	CloseHandle(hEventNS);
	CloseHandle(hEventEV);
	CloseHandle(hEventMaster);
}

void randomStuf()
{
	srand(GetTickCount());
	ThrDir.assign(numberOfThreads, 0);
	for (auto& i : ThrDir)
	{
		i = rand() % 4;
	}
	for (auto i : ThrDir)
		std::cout << i << " ";
}

