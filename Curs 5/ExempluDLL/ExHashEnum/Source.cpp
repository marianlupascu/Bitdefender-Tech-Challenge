#include <Windows.h>
#include <stdio.h>
#include <Wincrypt.h>
#include <queue>

#define BUFSIZE 1024
#define MD5LEN  16

CRITICAL_SECTION csTasks;
CRITICAL_SECTION csPrint;
HANDLE hSemaphore;
std::queue<wchar_t *> myQueue;

DWORD PrintHashForFile(wchar_t * wszFileName)
{
	DWORD dwStatus = 0;
	BOOL bResult = FALSE;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	HANDLE hFile = NULL;
	BYTE rgbFile[BUFSIZE];
	DWORD cbRead = 0;
	BYTE rgbHash[MD5LEN];
	DWORD cbHash = 0;
	CHAR rgbDigits[] = "0123456789abcdef";
	//LPCWSTR filename = L"filename.txt";
	// Logic to check usage goes here.

	hFile = CreateFile(wszFileName,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		dwStatus = GetLastError();
//		printf("Error opening file %s\nError: %d\n", wszFileName, dwStatus);
		return dwStatus;
	}

	// Get handle to the crypto provider
	if (!CryptAcquireContext(&hProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT))
	{
		dwStatus = GetLastError();
		printf("CryptAcquireContext failed: %d\n", dwStatus);
		CloseHandle(hFile);
		return dwStatus;
	}

	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	{
		dwStatus = GetLastError();
		printf("CryptAcquireContext failed: %d\n", dwStatus);
		CloseHandle(hFile);
		CryptReleaseContext(hProv, 0);
		return dwStatus;
	}

	while (bResult = ReadFile(hFile, rgbFile, BUFSIZE,
		&cbRead, NULL))
	{
		if (0 == cbRead)
		{
			break;
		}

		if (!CryptHashData(hHash, rgbFile, cbRead, 0))
		{
			dwStatus = GetLastError();
			//printf("CryptHashData failed: %d\n", dwStatus);
			CryptReleaseContext(hProv, 0);
			CryptDestroyHash(hHash);
			CloseHandle(hFile);
			return dwStatus;
		}
	}

	if (!bResult)
	{
		dwStatus = GetLastError();
		//printf("ReadFile failed: %d\n", dwStatus);
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CloseHandle(hFile);
		return dwStatus;
	}

	cbHash = MD5LEN;
	if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
	{
		EnterCriticalSection(&csPrint);
		wprintf(L"MD5 hash of file %s is: ", wszFileName);
		for (DWORD i = 0; i < cbHash; i++)
		{
			printf("%c%c", rgbDigits[rgbHash[i] >> 4],
				rgbDigits[rgbHash[i] & 0xf]);
		}
		printf("\n");
		LeaveCriticalSection(&csPrint);
	}
	else
	{
		dwStatus = GetLastError();
		//printf("CryptGetHashParam failed: %d\n", dwStatus);
	}

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	CloseHandle(hFile);

	return dwStatus;
}


DWORD WINAPI FuncThread(LPVOID param) {

	while (1) {

		wchar_t * wszFile = NULL;

		WaitForSingleObject(hSemaphore, INFINITE);
		EnterCriticalSection(&csTasks);
		wszFile = myQueue.front();
		myQueue.pop();
		LeaveCriticalSection(&csTasks);
		if (!wcscmp(wszFile, L"")) {
			free(wszFile);
			break;
		}
		free(wszFile);
		PrintHashForFile(wszFile);
	}

	return 0;
}


int Enumerate(wchar_t * wszFolderToEnumerate)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	wchar_t szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;



	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.

	length_of_arg = wcsnlen_s(wszFolderToEnumerate, MAX_PATH);

	if (length_of_arg > (MAX_PATH - 3))
	{
		wprintf(L"Directory path is too long.\n");
		return (-1);
	}

	//wprintf(L"Target directory is %s\n", wszFolderToEnumerate);

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	wcscpy_s(szDir, MAX_PATH, wszFolderToEnumerate);
	wcscat_s(szDir, MAX_PATH, L"\\*");

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		wprintf(L"FindFirstFile");
		return dwError;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!wcscmp(ffd.cFileName, L".") ||
				!wcscmp(ffd.cFileName, L".."))
				continue;
			//wprintf(L"  %s   <DIR>\n", ffd.cFileName);
			wchar_t wszRecursiveFolder[MAX_PATH];
			wcscpy_s(wszRecursiveFolder, wszFolderToEnumerate);
			wcscat_s(wszRecursiveFolder, L"\\");
			wcscat_s(wszRecursiveFolder, ffd.cFileName);
			Enumerate(wszRecursiveFolder);
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			wchar_t wszFullName[MAX_PATH];
			wcscpy_s(wszFullName, wszFolderToEnumerate);
			wcscat_s(wszFullName, L"\\");
			wcscat_s(wszFullName, ffd.cFileName);

			wchar_t * wszDup = _wcsdup(wszFullName);
			EnterCriticalSection(&csTasks);
			//myQueue.push(wszFullName);
			myQueue.push(wszDup);
			LeaveCriticalSection(&csTasks);
			LONG lpPrev = 0;
			ReleaseSemaphore(hSemaphore, 1, &lpPrev);
			//wprintf(L"  %s   %ld bytes\n", ffd.cFileName, filesize.QuadPart);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		wprintf(L"FindFirstFile- something went wrong last error %u", dwError);
	}

	FindClose(hFind);
	return dwError;
}

int wmain(int argc, wchar_t * argv[])
{

	//PrintHashForFile(L"d:\\out.txt");

	InitializeCriticalSection(&csTasks);
	InitializeCriticalSection(&csPrint);
	hSemaphore = CreateSemaphore(NULL, 0, INT_MAX, NULL);
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	int nrThreads = systemInfo.dwNumberOfProcessors;
	HANDLE *hThreads = (HANDLE *)malloc(nrThreads * sizeof(HANDLE));
	DWORD * dwThreadsID = (DWORD *)malloc(nrThreads * sizeof(DWORD));

	for (int i = 0; i < nrThreads; i++) {
		hThreads[i] = CreateThread(NULL, 0, FuncThread, NULL, 0, &dwThreadsID[i]);
	}

	Enumerate(argv[1]);
	for (int i = 0; i < nrThreads; i++) {

		EnterCriticalSection(&csTasks);
		wchar_t * myFake = _wcsdup(L"");
		myQueue.push(myFake);
		LeaveCriticalSection(&csTasks);
		LONG lpPrevCount = 0;
		ReleaseSemaphore(hSemaphore, 1, &lpPrevCount);
	}


	for (int i = 0; i < nrThreads; i++)
		WaitForSingleObject(hThreads[i], INFINITE);

	//WaitForMultipleObjects(nrThreads, hThreads, TRUE, INFINITE);

	LeaveCriticalSection(&csTasks);
	LeaveCriticalSection(&csPrint);

	return 0;
}