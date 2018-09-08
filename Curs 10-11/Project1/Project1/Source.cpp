#include <Windows.h>
#include <iostream>

int wmain(int argc, wchar_t* argv[])
{
	if (argc < 2)
	{
		std::wcout << L"Nu avem param\n";
		return -1;
	}

	const HANDLE hFile = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE,
	                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
	                          nullptr,
	                          OPEN_EXISTING,
	                          0,
	                          nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"Eroare la handle\n";
		return -2;
	}

	const HANDLE hMapping = CreateFileMapping(hFile,
	                                    nullptr, PAGE_READWRITE,
	                                    0,
	                                    100,
	                                    nullptr);

	if (hMapping == nullptr)
	{
		std::wcout << L"nu s-a creat mapping-ul\n";
		return -3;
	}

	void* pMap = MapViewOfFile(hMapping,
	                           FILE_MAP_READ | FILE_MAP_WRITE,
	                           0,
	                           0,
	                           0);

	if (pMap == nullptr)
	{
		std::wcout << L"nu putem obtine pointer la zona mapata\n";
		std::wcout << GetLastError() << L"\n";
		return -4;
	}

	for (auto i = 0; i < 25; i++)
	{
		static_cast<char*>(pMap)[i] = 'a' + i;
	}

	FlushViewOfFile(pMap, 25); 

	UnmapViewOfFile(pMap);
	CloseHandle(hMapping);

	return 0;
}
