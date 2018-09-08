#include <Windows.h>
#include <iostream>

int main()
{
	LPCWSTR name = L"Global\\Sem__";

	HANDLE hSem = OpenSemaphore(SYNCHRONIZE, FALSE, name);
	if (hSem != nullptr)
	{
		hSem = CreateSemaphore(nullptr, 2, 2, name);
		ReleaseSemaphore(hSem, 1, nullptr);
		std::cout << "am creat semafor\n";
		getchar();
		Sleep(100000);
		//close mutex handle
		CloseHandle(hSem);
	}
	else
	{
		std::cout << "ruleaza...";
		DWORD var = WaitForSingleObject(hSem, 0);
		if (var == WAIT_OBJECT_0)
		{
			getchar();
			Sleep(60000);
			ReleaseSemaphore(hSem, 1, nullptr);
		}
		else
		{
			ExitProcess(0);
		}
	}


	getchar();

	return 0;
}
