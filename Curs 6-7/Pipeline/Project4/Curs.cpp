#include <Windows.h>
#include <iostream>

int main()
{
	LPCWSTR name = L"Global\\Mutex__";

	HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, name);
	if (hMutex != NULL)
	{
		HANDLE hMutex = CreateMutex(NULL, TRUE, name);
		std::cout << "am creat\n";
	}
	else
	{
		std::cout << "ruleaza\n";
	}

	//close mutex handle
	CloseHandle(hMutex);

	getchar();

	return 0;
}
