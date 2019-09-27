/*
 * ╓──────────────────────────────────────────────────────────────────────────────────────╖
 * ║                                                                                      ║
 * ║   WinAPIEx                                                                           ║
 * ║                                                                                      ║
 * ║   Copyright (c) 2018, bytecode77                                                     ║
 * ║   All rights reserved.                                                               ║
 * ║                                                                                      ║
 * ║   Version 0.8.4                                                                      ║
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

#include "../WinAPIEx/WinAPIEx.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	BOOL result = FALSE;
	C::Array<LPWSTR> arguments = *C::Environment::GetCommandLineArgs();

	LPWSTR commandLine = NULL;

	if (arguments.Count() <= 1)
	{
		commandLine = new WCHAR[100];
		StrCpyW(commandLine, L"C:\\Windows\\System32\\cmd.exe /T:4F");
	}
	else
	{
		commandLine = arguments[1];
	}

	DWORD processId;
	if (C::Process::CreateProcessWithIntegrity(commandLine, SECURITY_MANDATORY_LOW_RID, &processId)) result = TRUE;

	return result ? 0 : 1;
}