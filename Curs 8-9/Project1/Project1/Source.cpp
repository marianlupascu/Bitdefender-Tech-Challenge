#include <Windows.h>
#include <cstdio>
#include <iostream>

int EnumerateAllServices(SC_HANDLE hSCM) {
	void* buf = NULL;
	DWORD bufSize = 0;
	DWORD moreBytesNeeded, serviceCount;
	for (;;) {
		printf("Calling EnumServiceStatusEx with bufferSize %d\n", bufSize);
		if (EnumServicesStatusEx(
			hSCM,
			SC_ENUM_PROCESS_INFO,
			SERVICE_WIN32,
			SERVICE_STATE_ALL,
			(LPBYTE)buf,
			bufSize,
			&moreBytesNeeded,
			&serviceCount,
			NULL,
			NULL)) {
			ENUM_SERVICE_STATUS_PROCESS* services = (ENUM_SERVICE_STATUS_PROCESS*)buf;
			for (DWORD i = 0; i < serviceCount; ++i) {
				std::wcout << services[i].lpServiceName<<'\n';
			}
			free(buf);
			return 0;
		}
		int err = GetLastError();
		if (ERROR_MORE_DATA != err) {
			free(buf);
			return err;
		}
		bufSize += moreBytesNeeded;
		free(buf);
		buf = malloc(bufSize);
	}
}

int main() {
	WORD pId = GetCurrentProcessId();
	SC_HANDLE hSCM = NULL;
	PUCHAR  pBuf = NULL;
	ULONG  dwBufSize = 0x00;
	ULONG  dwBufNeed = 0x00;
	ULONG  dwNumberOfService = 0x00;
	LPENUM_SERVICE_STATUS_PROCESS pInfo = NULL;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_CONNECT);

	if (hSCM == NULL)
	{
		return 0;
	}
	EnumerateAllServices(hSCM);
	getchar();
	return 0;
}