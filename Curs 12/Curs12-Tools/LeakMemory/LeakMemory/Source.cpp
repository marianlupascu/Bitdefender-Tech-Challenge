//#include <vld.h>
#include <Windows.h>


int main()
{
	while (1)
	{
		HANDLE hFile = CreateFile(L"d:\\out_.txt",
			GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_EXISTING, 0, NULL);
		Sleep(1000);

	}

	return 0;
}