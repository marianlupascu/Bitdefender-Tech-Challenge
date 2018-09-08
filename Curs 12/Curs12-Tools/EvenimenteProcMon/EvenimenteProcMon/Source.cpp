#include <Windows.h>
#include <stdio.h>

#define FILE_DEVICE_PROCMON_LOG     0x00009535
#define IOCTL_EXTERNAL_LOG_DEBUGOUT	(ULONG) CTL_CODE( FILE_DEVICE_PROCMON_LOG ,\
                                                      0x81                    ,\
                                                      METHOD_BUFFERED         ,\
                                                      FILE_WRITE_ACCESS        )

HANDLE hDevice = INVALID_HANDLE_VALUE;


int ProcMonLog(const wchar_t* string)
{
	//	When runtime flags are OFF open file on constructor
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD dwLen;
		DWORD dwOut;

		dwLen = (DWORD)wcslen(string) * sizeof(WCHAR);
		::DeviceIoControl(hDevice,
			IOCTL_EXTERNAL_LOG_DEBUGOUT,
			(LPVOID)string,
			dwLen,
			NULL,
			0,
			&dwOut,
			NULL);
	}
	return 0;
}

int OpenProcMon()
{
	hDevice = ::CreateFileW(L"\\\\.\\Global\\ProcmonDebugLogger",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Error opening proc mon log.\n");
		return -1;
	}

	
	return 0;
}

int CloseProcMon()
{
	CloseHandle(hDevice);
	return 0;
}
int main()
{
	OpenProcMon();
	HANDLE h = CreateFile(L"d:\\out_.txt", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	ProcMonLog(L"Acesta e logul meu 1");
	CloseHandle(h);
	ProcMonLog(L"Acesta e logul meu 2");
	CloseProcMon();
	return 0;
}