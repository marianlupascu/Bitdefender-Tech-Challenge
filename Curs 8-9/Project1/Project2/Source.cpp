#include <Windows.h>
#include <cstdio>
#include <iostream>

int main() {
	WORD pId = GetCurrentProcessId();
	SC_HANDLE scm = NULL;
	PUCHAR  pBuf = NULL;
	ULONG  dwBufSize = 0x00;
	ULONG  dwBufNeed = 0x00;
	ULONG  dwNumberOfService = 0x00;
	LPENUM_SERVICE_STATUS_PROCESS pInfo = NULL;

	scm = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_CONNECT);

	if (scm == NULL)
	{
		std::cout << "scm nullptr\n";
		getchar();
		return 0;
	}

	SC_HANDLE svc = OpenService(scm, (L"TeamViewer"), 0);
	int lastError = GetLastError();

	if (ERROR_SERVICE_DOES_NOT_EXIST == lastError) {
		std::cout << "ERROR_SERVICE_DOES_NOT_EXIST";
		getchar();
		return 0;
	}


		SERVICE_STATUS status;
		QueryServiceStatus(svc, &status);
		if (SERVICE_STOPPED != status.dwCurrentState) {
			if (ControlService(svc, SERVICE_CONTROL_STOP, &status)) {
				do {
					QueryServiceStatus(svc, &status);
				} while (SERVICE_STOPPED != status.dwCurrentState);
			}
			else {
				/* Handle */
			}
		}
	

	CloseServiceHandle(svc);
	CloseServiceHandle(scm);


	getchar();
	return 0;
}