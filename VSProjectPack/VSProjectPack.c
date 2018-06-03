#include <Windows.h>
STARTUPINFO si;
PROCESS_INFORMATION pi;

PWSTR GetArg1W(PWSTR s)
{
	PWSTR sC;

	for (; *s; ++s)
		if (*s == ' ')
			break;

	if (*s)
	{
		sC = s;
		for (; *s; ++s)
			if (*s == '\"')
				*s = ' ';

		for (;;)
		{
			--s;
			if (*s != ' ')
				break;
			else
				*s = '\0';
		}

		while (*++sC == ' ');

		if (!*sC)
			return NULL;
		else
			return sC;
	}
	else
		return NULL;
}
BOOL RemoveLastPossibleBSlash(PWSTR s)
{
	for (; *s; ++s) {}
	--s;
	if (*s == '\\')
	{
		*s = '\0';
		return TRUE;
	}
	return FALSE;
}
VOID CleanVSProjectDirectory(WCHAR* sDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFile;
	WCHAR sPath[MAX_PATH];
	WCHAR* sSlash;
	WCHAR* sDot;
	wcscpy_s(sPath, MAX_PATH, sDirectory);
	wcscat_s(sPath, MAX_PATH, L"\\*");
	sSlash = wcsrchr(sPath, '*');
	hFile = FindFirstFile(sPath, &FindFileData);
	do
	{
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			wcscpy_s(sSlash, MAX_PATH, FindFileData.cFileName);
			sDot = wcsrchr(FindFileData.cFileName, '.');
			if (sDot)
			{
				if (!_wcsicmp(sDot, L".obj") || !_wcsicmp(sDot, L".pdb") || !_wcsicmp(sDot, L".pch")
					|| !_wcsicmp(sDot, L".ipch") || !_wcsicmp(sDot, L".db") || !_wcsicmp(sDot, L".ilk"))
				{
					DeleteFile(sPath);
				}
			}
		}
		else if (wcscmp(FindFileData.cFileName, L".") && wcscmp(FindFileData.cFileName, L".."))
		{
			wcscpy_s(sSlash, MAX_PATH, FindFileData.cFileName);
			CleanVSProjectDirectory(sPath);
		}
	} while (FindNextFile(hFile, &FindFileData));
}
VOID PackProject(WCHAR* sDirectory)
{
	WCHAR sCMD[MAX_PATH];
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;
	wsprintf(sCMD, L"7za a -r %s.zip %s\\*", wcsrchr(sDirectory, '\\') + 1, sDirectory);
	// MessageBox(NULL, sCMD, L"CMD", MB_OK);
	CreateProcess(NULL, sCMD, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}
VOID EntryPoint()
{
	PWSTR sArg1 = GetArg1W(GetCommandLine());
	BOOL bIsRoot;

	bIsRoot = RemoveLastPossibleBSlash(sArg1);
	CleanVSProjectDirectory(sArg1);
	if (bIsRoot == FALSE)
		PackProject(sArg1);
	ExitProcess(0);
}