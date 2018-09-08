#include <Windows.h>
#include <iostream>
#include <string>

HANDLE sem1 = nullptr;
HANDLE sem2 = nullptr;

DWORD WINAPI Fun1(LPVOID);
DWORD WINAPI Fun2(LPVOID);

int a = 1;

int wmain(int argc, wchar_t* argv[])
{

	HANDLE hThread1;
	HANDLE hThread2;
	sem1 = CreateSemaphore(NULL, 1, 1, NULL);
	sem2 = CreateSemaphore(NULL, 0, 1, NULL);

	hThread1 = CreateThread(nullptr,
		0,
		Fun1,
		nullptr,
		0,
		NULL);

	hThread2 = CreateThread(nullptr,
		0,
		Fun2,
		nullptr,
		0,
		NULL);
	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	getchar();
	return 0;
}

DWORD WINAPI Fun1(LPVOID param)
{
	while (true)
	{
		WaitForSingleObject(sem1, INFINITE);
		std::cout << a << '\n';
		a++;
		if (a >= 100)
		{
			ReleaseSemaphore(sem2, 1, NULL);
			break;
		}
		ReleaseSemaphore(sem2, 1, NULL);
	}
	return  0;
}

DWORD WINAPI Fun2(LPVOID param)
{
	while (true)
	{
		WaitForSingleObject(sem2, INFINITE);
		std::cout << a << '\n';
		a++;
		if (a >= 100)
		{
			ReleaseSemaphore(sem1, 1, NULL);
			break;
		}
		ReleaseSemaphore(sem1, 1, NULL);
	}
	return  0;
}