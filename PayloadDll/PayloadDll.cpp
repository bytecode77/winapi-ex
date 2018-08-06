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

#include "..\WinAPIEx\WinAPIEx.h"

LPWSTR GetTime()
{
	time_t currentTime = time(NULL);
	struct tm *timeInfo = localtime(&currentTime);

	WCHAR buffer[50];
	DWORD size = (DWORD)wcsftime(buffer, sizeof(buffer), L"%Y.%m.%d %H:%M:%S", timeInfo);

	PWCHAR result = new WCHAR[size + 1];
	StrCpyW(result, buffer);

	return result;
}
LPWSTR GetIntegrityString(HANDLE process)
{
	DWORD integrityLevel = C::Process::GetProcessIntegrityLevel(process);
	LPCWSTR integrityLevelName = C::Process::GetIntegrityLevelName(integrityLevel);

	WCHAR integrityString[50] = L"";
	if (integrityLevel != -1)
	{
		StrCatW(integrityString, L"0x");
		StrCatW(integrityString, C::Convert::Int32ToString(integrityLevel, 16));

		if (integrityLevelName)
		{
			StrCatW(integrityString, L" ");
			StrCatW(integrityString, integrityLevelName);
		}
	}

	PWCHAR result = new WCHAR[lstrlenW(integrityString) + 1];
	StrCpyW(result, integrityString);

	return result;
}
void WriteValues(HKEY hive, HINSTANCE hInstance)
{
	if (C::Registry::CreateKey(hive, L"SOFTWARE", L"__PAYLOAD_DLL"))
	{
		C::Array<LPWSTR> *keys = C::Registry::GetKeyNames(hive, L"SOFTWARE\\__PAYLOAD_DLL");

		int currentIndex = 0;
		if (keys)
		{
			for (int i = 0; i < keys->Count; i++)
			{
				int index = C::Convert::StringToInt32(keys->Values[i]);
				if (index > currentIndex) currentIndex = index;
			}
		}

		WCHAR newName[16] = L"000000";
		LPWSTR currentIndexString = C::Convert::Int32ToString(currentIndex + 1);
		StrCpyW(&newName[6 - lstrlenW(currentIndexString)], currentIndexString);

		LPWSTR randomString = C::Random::HexadecimalString(6);
		if (randomString)
		{
			StrCatW(newName, L"...");
			StrCatW(newName, randomString);
		}

		if (C::Registry::CreateKey(hive, L"SOFTWARE\\__PAYLOAD_DLL", newName))
		{
			WCHAR newPath[50] = L"SOFTWARE\\__PAYLOAD_DLL\\";
			StrCatW(newPath, newName);

			LPWSTR time = GetTime();
			LPWSTR path = C::Path::GetExecutablePath();
			LPWSTR module = C::Path::GetModulePath(hInstance);
			LPWSTR user = C::Environment::GetCurrentUser();

			LPWSTR integrityLevel = NULL;
			HANDLE process = GetCurrentProcess();
			if (process)
			{
				integrityLevel = GetIntegrityString(process);
				CloseHandle(process);
			}

			DWORD parentProcessId = C::Process::GetParentProcessId(GetCurrentProcessId());
			LPWSTR parentName = parentProcessId == 0 ? NULL : C::Process::GetProcessName(parentProcessId);

			LPWSTR parentIntegrityLevel = NULL;
			if (parentProcessId != 0)
			{
				HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, parentProcessId);
				if (process)
				{
					parentIntegrityLevel = GetIntegrityString(process);
					CloseHandle(process);
				}
			}

			LPWSTR parentCommandLine = parentProcessId == 0 ? NULL : C::Process::GetProcessCommandLine(parentProcessId);

			C::Registry::SetValueString(hive, newPath, L"00_Time", time ? time : L"");

			C::Registry::SetValueString(hive, newPath, L"10_Path", path ? path : L"");
			C::Registry::SetValueString(hive, newPath, L"11_CommandLine", GetCommandLineW());
			C::Registry::SetValueString(hive, newPath, L"12_Module", module ? module : L"");

			C::Registry::SetValueDword(hive, newPath, L"20_PID", GetCurrentProcessId());
			C::Registry::SetValueString(hive, newPath, L"21_Integrity", integrityLevel ? integrityLevel : L"");
			C::Registry::SetValueString(hive, newPath, L"22_User", user ? user : L"");

			C::Registry::SetValueDword(hive, newPath, L"30_Parent PID", parentProcessId);
			C::Registry::SetValueString(hive, newPath, L"31_Parent Name", parentName ? parentName : L"");
			C::Registry::SetValueString(hive, newPath, L"32_Parent Integrity", parentIntegrityLevel ? parentIntegrityLevel : L"");
			C::Registry::SetValueString(hive, newPath, L"33_Parent CommandLine", parentCommandLine ? parentCommandLine : L"");
		}
	}
}

bool WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		WriteValues(HKEY_CURRENT_USER, hInstDll);
		WriteValues(HKEY_LOCAL_MACHINE, hInstDll);
	}

	return false;
}
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WriteValues(HKEY_CURRENT_USER, hInstance);
	WriteValues(HKEY_LOCAL_MACHINE, hInstance);

	return 0;
}