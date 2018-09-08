#include <Windows.h>

typedef int(*MYFUNC)(int param);

int main()
{
	HMODULE h = LoadLibrary(L"MyDll.dll");


	MYFUNC p = (MYFUNC)GetProcAddress(h, "MyBadFunction");

	p(GetTickCount());

	FreeLibrary(h);
	return 0;
}