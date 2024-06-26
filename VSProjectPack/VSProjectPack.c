#include <Windows.h>
#include <stdio.h>

INT MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
INT MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText,	IN LPCWSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

#ifdef UNICODE
#define MessageBoxTimeout MessageBoxTimeoutW
#else
#define MessageBoxTimeout MessageBoxTimeoutA
#endif

STARTUPINFO si;
PROCESS_INFORMATION pi;
WCHAR 项目文件名[MAX_PATH];

VOID RemoveLastPossibleBSlash(PWSTR s)
{
	for (; *s; ++s) {}
	--s;
	if (*s == '\\')
	{
		*s = '\0';
	}
}
VOID 获取项目文件名(PWSTR p项目文件名)
{
	++p项目文件名;
	wcscpy_s(项目文件名, MAX_PATH, p项目文件名);
	wcscat_s(项目文件名, MAX_PATH, L".sln");
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
				if (!_wcsicmp(sDot, L".obj") || !_wcsicmp(sDot, L".iobj") || !_wcsicmp(sDot, L".pdb") || !_wcsicmp(sDot, L".ipdb") || !_wcsicmp(sDot, L".pch") || !_wcsicmp(sDot, L".ipch")
					|| !_wcsicmp(sDot, L".bsc") || !_wcsicmp(sDot, L".db") || !_wcsicmp(sDot, L".ilk") || !_wcsicmp(sDot, L".res") || !_wcsicmp(sDot, L".log")
					|| !_wcsicmp(sDot, L".tlog") || !_wcsicmp(sDot, L".aps"))
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
	swprintf_s(sCMD, MAX_PATH, L"7za a -r -xr!.git %s.7z %s\\*", wcsrchr(sDirectory, '\\') + 1, sDirectory);

	CreateProcess(NULL, sCMD, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}
VOID EntryPoint()
{
	PWSTR* sArgv;
	INT nArgs;

	sArgv = CommandLineToArgvW(GetCommandLine(), &nArgs);
	if (sArgv)
	{
		if (nArgs > 1 && nArgs <= 3)
		{
			PWSTR s目录名;
			HANDLE hProjFile;
			PWSTR p反斜杠;

			s目录名 = sArgv[1];
			RemoveLastPossibleBSlash(s目录名);
			p反斜杠 = wcsrchr(s目录名, '\\');
			if (p反斜杠)
			{
				获取项目文件名(p反斜杠);
				hProjFile = CreateFile(项目文件名, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (hProjFile != INVALID_HANDLE_VALUE)
				{
					CloseHandle(hProjFile);
					CleanVSProjectDirectory(s目录名);
					if (nArgs == 2)
						PackProject(s目录名);
					MessageBoxTimeout(NULL, L"Done", L"VSProjectPack", MB_OK | MB_ICONINFORMATION, 0, 1000);
				}
				else
					MessageBoxTimeout(NULL, L"Not in a VSProject folder", L"VSProjectPack", MB_OK | MB_ICONINFORMATION, 0, 1500);
			}
		}

		LocalFree(sArgv);
	}

	ExitProcess(0);
}