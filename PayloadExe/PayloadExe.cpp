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

LPWSTR GetIntegrityString(HANDLE process, PDWORD integrityLevel)
{
	*integrityLevel = C::Process::GetProcessIntegrityLevel(process);
	LPCWSTR integrityLevelName = C::Process::GetIntegrityLevelName(*integrityLevel);

	WCHAR integrityString[50] = L"";
	if (*integrityLevel != -1)
	{
		StrCatW(integrityString, L"0x");
		StrCatW(integrityString, C::Convert::Int32ToString(*integrityLevel, 16));

		if (integrityLevelName)
		{
			StrCatW(integrityString, L" ");
			StrCatW(integrityString, integrityLevelName);
		}
	}
	else
	{
		StrCatW(integrityString, L"?");
	}

	PWCHAR result = new WCHAR[lstrlenW(integrityString) + 1];
	StrCpyW(result, integrityString);

	return result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LPWSTR path = C::Path::GetExecutablePath();
	C::Array<LPWSTR> *commandLine = C::Environment::GetCommandLineArgs();
	LPWSTR user = C::Environment::GetCurrentUser();

	DWORD integrityLevel = -1;
	LPWSTR integrityLevelName = NULL;
	HANDLE process = GetCurrentProcess();
	if (process)
	{
		integrityLevelName = GetIntegrityString(process, &integrityLevel);
		CloseHandle(process);
	}

	WCHAR message[16384] = L"";
	StrCatW(message, L"Example Payload (");
	StrCatW(message, sizeof(size_t) == 8 ? L"x64" : L"x86");
	StrCatW(message, L")\n\n");

	StrCatW(message, path ? path : L"?");
	StrCatW(message, L"\n\n");

	StrCatW(message, L"CommandLine Arguments:");
	if (commandLine)
	{
		if (commandLine->Count <= 1)
		{
			StrCatW(message, L" (none)\n");
		}
		else
		{
			StrCatW(message, L"\n");

			for (int i = 1; i < (int)commandLine->Count; i++)
			{
				StrCatW(message, L"[");
				StrCatW(message, C::Convert::Int32ToString(i));
				StrCatW(message, L"] = ");
				StrCatW(message, commandLine->Values[i]);
				StrCatW(message, L"\n");
			}
		}
	}
	StrCatW(message, L"\n");

	StrCatW(message, L"PID: ");
	StrCatW(message, C::Convert::Int32ToString(GetCurrentProcessId()));
	StrCatW(message, L"\n");

	StrCatW(message, L"Integrity: ");
	StrCatW(message, integrityLevelName);
	StrCatW(message, L"\n");

	StrCatW(message, L"User: ");
	StrCatW(message, user ? user : L"?");
	StrCatW(message, L"\n");

	C::Message::Information(message);

	return 0;
}