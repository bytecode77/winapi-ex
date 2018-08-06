/*
 * ╓──────────────────────────────────────────────────────────────────────────────────────╖
 * ║                                                                                      ║
 * ║   WinAPIEx                                                                           ║
 * ║                                                                                      ║
 * ║   Copyright (c) 2018, bytecode77                                                     ║
 * ║   All rights reserved.                                                               ║
 * ║                                                                                      ║
 * ║   Version 0.8.1                                                                      ║
 * ║   https://bytecode77.com/framework/winapi-ex                                         ║
 * ║                                                                                      ║
 * ╟──────────────────────────────────────────────────────────────────────────────────────╢
 * ║                                                                                      ║
 * ║   Redistribution and use in source and binary forms, with or without                 ║
 * ║   modification, are permitted provided that the following conditions are met:        ║
 * ║                                                                                      ║
 * ║   * Redistributions of source code must retain the above copyright notice, this      ║
 * ║     list of conditions and the following disclaimer.                                 ║
 * ║                                                                                      ║
 * ║   * Redistributions in binary form must reproduce the above copyright notice, this   ║
 * ║     list of conditions and the following disclaimer in the documentation and/or      ║
 * ║     other materials provided with the distribution.                                  ║
 * ║                                                                                      ║
 * ║   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND    ║
 * ║   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED      ║
 * ║   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE             ║
 * ║   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR   ║
 * ║   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES     ║
 * ║   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;       ║
 * ║   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON     ║
 * ║   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT            ║
 * ║   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS      ║
 * ║   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                       ║
 * ║                                                                                      ║
 * ╙──────────────────────────────────────────────────────────────────────────────────────╜
 */

#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "netapi32.lib")

#include <fstream>
#include <Windows.h>
#include <winternl.h>
#include <shlobj.h>
#include <Shlwapi.h>
#include <TlHelp32.h>
#include <lm.h>
#include <Psapi.h>
#include <time.h>

namespace C
{
	//TODO: Review checking of invalid handles (NULL vs INVALID_HANDLE_VALUE, INVALID_SOMETHING_HANDLE, etc.)
	//FEATURE: Injection bootstrapper for .NET DLL's

	template <typename T>
	struct Array
	{
		int Count;
		T* Values;

		Array(int count)
		{
			Count = count;
			Values = new T[count];
		}
	};

	enum class SpecialFolder
	{
		Desktop = CSIDL_DESKTOP,
		Internet = CSIDL_INTERNET,
		Programs = CSIDL_PROGRAMS,
		Controls = CSIDL_CONTROLS,
		Printers = CSIDL_PRINTERS,
		MyDocuments = CSIDL_PERSONAL,
		Favorites = CSIDL_FAVORITES,
		Startup = CSIDL_STARTUP,
		Recent = CSIDL_RECENT,
		SendTo = CSIDL_SENDTO,
		BitBucket = CSIDL_BITBUCKET,
		StartMenu = CSIDL_STARTMENU,
		MyMusic = CSIDL_MYMUSIC,
		MyVideo = CSIDL_MYVIDEO,
		DesktopDirectory = CSIDL_DESKTOPDIRECTORY,
		Drives = CSIDL_DRIVES,
		Network = CSIDL_NETWORK,
		NetHood = CSIDL_NETHOOD,
		Fonts = CSIDL_FONTS,
		Templates = CSIDL_TEMPLATES,
		CommonStartMenu = CSIDL_COMMON_STARTMENU,
		CommonPrograms = CSIDL_COMMON_PROGRAMS,
		CommonStartup = CSIDL_COMMON_STARTUP,
		CommonDesktopDirectory = CSIDL_COMMON_DESKTOPDIRECTORY,
		AppData = CSIDL_APPDATA,
		PrintHood = CSIDL_PRINTHOOD,
		LocalAppData = CSIDL_LOCAL_APPDATA,
		AltStartup = CSIDL_ALTSTARTUP,
		CommonAltStartup = CSIDL_COMMON_ALTSTARTUP,
		CommonFavorites = CSIDL_COMMON_FAVORITES,
		InternetCache = CSIDL_INTERNET_CACHE,
		Cookies = CSIDL_COOKIES,
		History = CSIDL_HISTORY,
		CommonAppData = CSIDL_COMMON_APPDATA,
		Windows = CSIDL_WINDOWS,
		System = CSIDL_SYSTEM,
		ProgramFiles = CSIDL_PROGRAM_FILES,
		MyPictures = CSIDL_MYPICTURES,
		Profile = CSIDL_PROFILE,
		SystemX86 = CSIDL_SYSTEMX86,
		ProgramFilesX86 = CSIDL_PROGRAM_FILESX86,
		ProgramFilesCommon = CSIDL_PROGRAM_FILES_COMMON,
		ProgramFilesCommonX86 = CSIDL_PROGRAM_FILES_COMMONX86,
		CommonTemplates = CSIDL_COMMON_TEMPLATES,
		CommonDocuments = CSIDL_COMMON_DOCUMENTS,
		CommonAdminTools = CSIDL_COMMON_ADMINTOOLS,
		AdminTools = CSIDL_ADMINTOOLS,
		Connections = CSIDL_CONNECTIONS,
		CommonMusic = CSIDL_COMMON_MUSIC,
		CommonPictures = CSIDL_COMMON_PICTURES,
		CommonVideo = CSIDL_COMMON_VIDEO,
		Resources = CSIDL_RESOURCES,
		ResourcesLocalized = CSIDL_RESOURCES_LOCALIZED,
		CommonOemLinks = CSIDL_COMMON_OEM_LINKS,
		CDBurnArea = CSIDL_CDBURN_AREA,
		ComputersNearMe = CSIDL_COMPUTERSNEARME
	};

	namespace Convert
	{
		LPWSTR StringToString(LPCSTR str);
		LPSTR StringToString(LPCWSTR str);

		LPWSTR Int32ToString(__int32 value, int base = 10);
		LPWSTR UInt32ToString(unsigned __int32 value, int base = 10);
		LPWSTR Int64ToString(__int64 value, int base = 10);
		LPWSTR UInt64ToString(unsigned __int64 value, int base = 10);

		__int32 StringToInt32(LPCWSTR str);
		//FEATURE: unsinged __int32 StringToUInt32(LPCWSTR str)
		__int64 StringToInt64(LPCWSTR str);
		//FEATURE: unsinged __int64 StringToUInt64(LPCWSTR str)
		float StringToFloat(LPCWSTR str);
		double StringToDouble(LPCWSTR str);

		LPWSTR UInt32ToHexString(unsigned __int32 value);
		LPWSTR UInt64ToHexString(unsigned __int64 value);
		LPWSTR BytesToHexView(LPBYTE data, DWORD length);
	}

	namespace Random
	{
		LPBYTE Bytes(DWORD length);
		LPWSTR HexadecimalString(DWORD length);
	}

	namespace Environment
	{
		LPWSTR GetCurrentUser();
		BOOL GetWindowsVersion(LPDWORD major, LPDWORD minor);
		Array<LPWSTR>* GetCommandLineArgs();
	}

	namespace Path
	{
		LPWSTR Combine(LPCWSTR path1, LPCWSTR path2);
		LPWSTR GetDirectoryName(LPCWSTR path);
		LPWSTR GetFileName(LPCWSTR path);

		LPWSTR GetExecutablePath();
		LPWSTR GetStartupPath();
		LPWSTR GetModulePath(HMODULE module);
		LPWSTR GetTempFolderPath();
		LPWSTR GetFolderPath(SpecialFolder folder);
	}

	namespace Directory
	{
		BOOL Exists(LPCWSTR path);
		Array<LPWSTR>* GetDirectories(LPCWSTR path);
		Array<LPWSTR>* GetFiles(LPCWSTR path);
	}

	namespace File
	{
		BOOL Exists(LPCWSTR path);
		LPBYTE Read(LPCWSTR path, LPDWORD bytesRead);
		BOOL Write(LPCWSTR path, LPBYTE data, DWORD length);
	}

	namespace Registry
	{
		LPWSTR GetValueString(HKEY hive, LPCWSTR subkey, LPCWSTR name);
		BOOL GetValueDword(HKEY hive, LPCWSTR subkey, LPCWSTR name, PDWORD value);
		BOOL SetValueString(HKEY hive, LPCWSTR subkey, LPCWSTR name, LPCWSTR value, BOOL isExpandedString = FALSE);
		BOOL SetValueDword(HKEY hive, LPCWSTR subkey, LPCWSTR name, DWORD value);
		BOOL DeleteValue(HKEY hive, LPCWSTR subkey, LPCWSTR name);

		BOOL CreateKey(HKEY hive, LPCWSTR subkey, LPCWSTR name);
		BOOL DeleteKey(HKEY hive, LPCWSTR subkey, LPCWSTR name);
		Array<LPWSTR>* GetKeyNames(HKEY hive, LPCWSTR subkey);
		Array<LPWSTR>* GetValueNames(HKEY hive, LPCWSTR subkey);
	}

	namespace Process
	{
		//FEATURE: BOOL EnableDebugPrivilege();
		LPCWSTR GetIntegrityLevelName(DWORD integrityLevel);

		DWORD GetProcessIdByName(LPCWSTR name);
		BOOL CreateProcessWithIntegrity(LPCWSTR commandLine, DWORD integrityLevel, LPDWORD processId);

		LPWSTR GetProcessName(DWORD processId);
		LPWSTR GetProcessCommandLine(DWORD processId);
		DWORD GetProcessIntegrityLevel(HANDLE process);
		DWORD GetParentProcessId(DWORD processId);
		BOOL InjectDll(HANDLE process, LPCWSTR dllPath);
	}

	//FEATURE: namespace Service { ... }

	namespace FileOperation
	{
		BOOL Copy(LPCWSTR srcPath, LPCWSTR destPath);
		BOOL Move(LPCWSTR srcPath, LPCWSTR destPath);
		BOOL Delete(LPCWSTR path);
	}

	namespace Message
	{
		void Information(LPCWSTR message);
		void Information(LPCWSTR title, LPCWSTR message);
		void Warning(LPCWSTR message);
		void Warning(LPCWSTR title, LPCWSTR message);
		void Error(LPCWSTR message, BOOL exitProcess = FALSE);
		void Error(LPCWSTR title, LPCWSTR message, BOOL exitProcess = FALSE);
		BOOL Confirmation(LPCWSTR message, BOOL warning = FALSE);
		BOOL Confirmation(LPCWSTR title, LPCWSTR message, BOOL warning = FALSE);
	}
}