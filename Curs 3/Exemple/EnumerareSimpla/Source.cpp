#include <Windows.h>
#include <stdio.h>
#include <string>

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

	wprintf(L"Target directory is %s\n", wszFolderToEnumerate);

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
			wprintf(L"  %s   <DIR>\n", ffd.cFileName);
			std::wstring wszFolderToEnumerateCopy = szDir;
			wszFolderToEnumerateCopy.pop_back();
			wszFolderToEnumerateCopy += ffd.cFileName;
			Enumerate((wchar_t*)wszFolderToEnumerateCopy.c_str());
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			wprintf(L"  %s   %ld bytes\n", ffd.cFileName, filesize.QuadPart);
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
	if (argc != 2)
	{
		wprintf(L"Usage: %s <directory name>\n", argv[0]);
		return (-1);
	}

	Enumerate(argv[1]);
	getchar();
	return 0;
}