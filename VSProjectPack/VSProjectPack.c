#include <Windows.h>

typedef int(__stdcall *MSGBOXAAPI)(IN HWND hWnd,
	IN LPCSTR lpText, IN LPCSTR lpCaption,
	IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
typedef int(__stdcall *MSGBOXWAPI)(IN HWND hWnd,
	IN LPCWSTR lpText, IN LPCWSTR lpCaption,
	IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

int MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText,
	IN LPCSTR lpCaption, IN UINT uType,
	IN WORD wLanguageId, IN DWORD dwMilliseconds);
int MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText,
	IN LPCWSTR lpCaption, IN UINT uType,
	IN WORD wLanguageId, IN DWORD dwMilliseconds);

#ifdef UNICODE
#define MessageBoxTimeout MessageBoxTimeoutW
#else
#define MessageBoxTimeout MessageBoxTimeoutA
#endif 

#define MB_TIMEDOUT 32000

STARTUPINFO si;
PROCESS_INFORMATION pi;
WCHAR 项目文件名[MAX_PATH];

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
					|| !_wcsicmp(sDot, L".bsc") || !_wcsicmp(sDot, L".db") || !_wcsicmp(sDot, L".ilk") || !_wcsicmp(sDot, L".res") || !_wcsicmp(sDot, L".exe") || !_wcsicmp(sDot, L".log")
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
	wsprintf(sCMD, L"7za a -r -xr!.git %s.zip %s\\*", wcsrchr(sDirectory, '\\') + 1, sDirectory);

	CreateProcess(NULL, sCMD, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}
VOID EntryPoint()
{
	PWSTR sArg1 = GetArg1W(GetCommandLine());
	HANDLE hProjFile;
	PWSTR p反斜杠;

	RemoveLastPossibleBSlash(sArg1);
	p反斜杠 = wcsrchr(sArg1, '\\');

	if (p反斜杠)
	{
		获取项目文件名(p反斜杠);
		hProjFile = CreateFile(项目文件名, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hProjFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hProjFile);
			CleanVSProjectDirectory(sArg1);
			PackProject(sArg1);
			MessageBoxTimeout(NULL, L"Done", L"VSProjectPack", MB_OK | MB_ICONINFORMATION, 0, 1000);
		}
		else
			MessageBoxTimeout(NULL, L"Not in a VSProject folder", L"VSProjectPack", MB_OK | MB_ICONINFORMATION, 0, 1000);
	}
	ExitProcess(0);
}