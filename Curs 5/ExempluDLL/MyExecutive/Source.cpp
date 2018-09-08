#include  "Header.h"
#include  <Windows.h>
#include <iostream>

int main()
{
	HMODULE hMyDLL = LoadLibrary(L"C:\\Users\\Marian\\Documents\\Documente\\Bitdefender Tech Challenge\\Curs 5\\ExempluDLN\\Debug\\ExempluDLL.dll");

	if(hMyDLL == NULL)
	{
		std::cout << "Biblioteca nu poate fi deschisa";
		return  -1;
	}

	AFISARE myFunc = (AFISARE)GetProcAddress(hMyDLL, "afisare");

	if(myFunc)
	{
		myFunc(100);
	}

	FreeLibrary(hMyDLL);

	getchar();

	return 0;
}
