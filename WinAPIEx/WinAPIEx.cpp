#include "WinAPIEx.h"

namespace C
{
	namespace Convert
	{
		void __TrimDecimalString(PWCHAR str)
		{
			int length = lstrlenW(str);
			for (; str[length - 1] == L'0'; length--)
			{
				str[length - 1] = L'\0';
			}

			if (str[length - 1] == L'.') str[length - 1] = L'\0';
		}

		LPWSTR StringToString(LPCSTR str)
		{
			int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
			PWCHAR result = new WCHAR[size];
			MultiByteToWideChar(CP_UTF8, 0, str, -1, result, size);

			return result;
		}
		LPSTR StringToString(LPCWSTR str)
		{
			int size = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
			PCHAR result = new CHAR[size];
			WideCharToMultiByte(CP_UTF8, 0, str, -1, result, size, NULL, NULL);

			return result;
		}

		LPWSTR Int32ToString(__int32 value, int base)
		{
			WCHAR buffer[20];
			_ltow_s(value, buffer, base);

			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}
		LPWSTR UInt32ToString(unsigned __int32 value, int base)
		{
			WCHAR buffer[20];
			_ultow_s(value, buffer, base);

			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}
		LPWSTR Int64ToString(__int64 value, int base)
		{
			WCHAR buffer[30];
			_i64tow_s(value, buffer, sizeof(buffer) / sizeof(WCHAR), base);

			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}
		LPWSTR UInt64ToString(unsigned __int64 value, int base)
		{
			WCHAR buffer[30];
			_ui64tow_s(value, buffer, sizeof(buffer) / sizeof(WCHAR), base);

			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}
		LPWSTR FloatToString(float value)
		{
			WCHAR buffer[50];
			swprintf_s(buffer, L"%f", value);

			__TrimDecimalString(buffer);
			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}
		LPWSTR DoubleToString(double value)
		{
			WCHAR buffer[50];
			swprintf_s(buffer, L"%.20f", value);

			__TrimDecimalString(buffer);
			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}

		__int32 StringToInt32(LPCWSTR str, int base)
		{
			return wcstol(str, NULL, base);
		}
		unsigned __int32 StringToUInt32(LPCWSTR str, int base)
		{
			return wcstoul(str, NULL, base);
		}
		__int64 StringToInt64(LPCWSTR str, int base)
		{
			return wcstoll(str, NULL, base);
		}
		unsigned __int64 StringToUInt64(LPCWSTR str, int base)
		{
			return wcstoull(str, NULL, base);
		}
		float StringToFloat(LPCWSTR str)
		{
			return wcstof(str, NULL);
		}
		double StringToDouble(LPCWSTR str)
		{
			return wcstod(str, NULL);
		}

		LPWSTR UInt32ToHexString(unsigned __int32 value)
		{
			LPWSTR buffer = UInt32ToString(value, 16);
			PWCHAR result = new WCHAR[11];

			StrCpyW(result, L"0x00000000");
			StrCpyW(&result[10 - lstrlenW(buffer)], buffer);
			delete buffer;

			return result;
		}
		LPWSTR UInt64ToHexString(unsigned __int64 value)
		{
			LPWSTR buffer = UInt64ToString(value, 16);
			PWCHAR result = new WCHAR[19];

			StrCpyW(result, L"0x0000000000000000");
			StrCpyW(&result[18 - lstrlenW(buffer)], buffer);
			delete buffer;

			return result;
		}
		LPWSTR BytesToHexView(LPBYTE data, DWORD length)
		{
			PWCHAR result = new WCHAR[((length - 1) / 16 + 1) * 79 + 1];

			for (DWORD i = 0, offset = 0; i < length; i += 16)
			{
				LPWSTR line = UInt32ToString(i, 16);
				StrCpyW(&result[offset], L"00000000");
				StrCpyW(&result[offset + 8 - lstrlenW(line)], line);
				StrCpyW(&result[offset + 8], L"h: ");

				delete line;
				offset += 11;

				for (DWORD j = 0; j < 16; j++)
				{
					if (i + j < length)
					{
						LPWSTR number = UInt32ToString(data[i + j], 16);
						StrCpyW(&result[offset], L"00");
						StrCpyW(&result[offset + 2 - lstrlenW(number)], number);
						result[offset + 2] = L' ';

						delete number;
					}
					else
					{
						result[offset] = L' ';
						result[offset + 1] = L' ';
						result[offset + 2] = L' ';
					}

					offset += 3;
				}

				result[offset++] = L';';
				result[offset++] = L' ';

				for (DWORD j = 0; j < 16; j++)
				{
					if (i + j < length)
					{
						result[offset++] = data[i + j] >= 32 && data[i + j] <= 126 ? (WCHAR)data[i + j] : L'.';
					}
					else
					{
						result[offset++] = L' ';
					}
				}

				StrCpyW(&result[offset], L"\r\n");
				offset += 2;
			}

			return result;
		}
	}

	namespace Random
	{
		LPBYTE Bytes(DWORD length)
		{
			HCRYPTPROV cryptProvider;
			if (!CryptAcquireContextW(&cryptProvider, NULL, L"Microsoft Base Cryptographic Provider v1.0", PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) return NULL;

			LPBYTE result = new BYTE[length];
			if (!CryptGenRandom(cryptProvider, length, result))
			{
				delete result;
				result = NULL;
			}

			CryptReleaseContext(cryptProvider, 0);
			return result;
		}
		LPWSTR HexadecimalString(DWORD length)
		{
			LPWSTR result = NULL;

			LPBYTE data = Bytes(length);
			if (data)
			{
				result = new WCHAR[length + 1];
				result[length] = L'\0';

				for (DWORD i = 0; i < length; i++) result[i] = L"0123456789abcdef"[data[i] & 15];
				delete data;
			}

			return result;
		}
	}

	namespace Environment
	{
		LPWSTR GetCurrentUser()
		{
			WCHAR buffer[UNLEN + 1];
			DWORD size = sizeof(buffer) / sizeof(WCHAR);
			if (!GetUserNameW(buffer, &size)) return NULL;

			PWCHAR result = new WCHAR[size];
			StrCpyW(result, buffer);

			return result;
		}
		BOOL GetWindowsVersion(LPDWORD major, LPDWORD minor)
		{
			LPBYTE buffer = NULL;
			if (NetWkstaGetInfo(NULL, 100, &buffer) != NERR_Success) return FALSE;

			WKSTA_INFO_100* workstationInfo = (LPWKSTA_INFO_100)buffer;
			*major = workstationInfo->wki100_ver_major;
			*minor = workstationInfo->wki100_ver_minor;
			NetApiBufferFree(buffer);

			return TRUE;
		}
		Array<LPWSTR>* GetCommandLineArgs()
		{
			int count;
			LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &count);

			Array<LPWSTR> *result = NULL;

			if (argv)
			{
				result = new Array<LPWSTR>(count);

				for (int i = 0; i < count; i++)
				{
					PWCHAR buffer = new WCHAR[lstrlenW(argv[i]) + 1];
					StrCpyW(buffer, argv[i]);
					result->Add(buffer);
				}

				LocalFree(argv);
			}

			return result;
		}
		LPWSTR GetTimeStamp(BOOL useFileFormat)
		{
			time_t currentTime = time(NULL);
			struct tm *timeInfo = localtime(&currentTime);

			WCHAR buffer[50];
			DWORD size = (DWORD)wcsftime(buffer, sizeof(buffer), useFileFormat ? L"%Y-%m-%d %H.%M.%S" : L"%Y-%m-%d %H:%M:%S", timeInfo);

			PWCHAR result = new WCHAR[size + 1];
			StrCpyW(result, buffer);

			return result;
		}
	}

	namespace Path
	{
		LPWSTR Combine(LPCWSTR path1, LPCWSTR path2)
		{
			WCHAR buffer[MAX_PATH + 1];
			if (!PathCombineW(buffer, path1, path2)) return FALSE;

			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}
		LPWSTR GetDirectoryName(LPCWSTR path)
		{
			PWCHAR buffer = new WCHAR[lstrlenW(path) + 1];
			StrCpyW(buffer, path);

			LPWSTR result = NULL;

			if (PathRemoveFileSpecW(buffer))
			{
				result = new WCHAR[lstrlenW(buffer) + 1];
				StrCpyW(result, buffer);
			}
			delete buffer;

			return result;
		}
		LPWSTR GetFileName(LPCWSTR path)
		{
			LPWSTR buffer = PathFindFileNameW(path);

			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}

		LPWSTR GetExecutablePath()
		{
			return GetModulePath(NULL);
		}
		LPWSTR GetStartupPath()
		{
			LPWSTR buffer = GetExecutablePath();
			if (!buffer) return NULL;

			wcsrchr(buffer, L'\\')[0] = L'\0';

			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);
			delete buffer;

			return result;
		}
		LPWSTR GetModulePath(HMODULE module)
		{
			WCHAR buffer[MAX_PATH + 1];
			if (!SUCCEEDED(GetModuleFileNameW(module, buffer, MAX_PATH + 1))) return NULL;

			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}
		LPWSTR GetTempFolderPath()
		{
			WCHAR buffer[MAX_PATH + 1];
			DWORD size = GetTempPathW(MAX_PATH + 1, buffer);

			if (buffer[size - 1] == L'\\') buffer[size - 1] = L'\0';

			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}
		LPWSTR GetFolderPath(SpecialFolder folder)
		{
			WCHAR buffer[MAX_PATH + 1];
			if (!SUCCEEDED(SHGetFolderPathW(NULL, (int)folder, NULL, SHGFP_TYPE::SHGFP_TYPE_CURRENT, buffer))) return NULL;

			PWCHAR result = new WCHAR[lstrlenW(buffer) + 1];
			StrCpyW(result, buffer);

			return result;
		}
	}

	namespace Directory
	{
		Array<LPWSTR>* __GetFilesOrDirectories(LPCWSTR path, BOOL getFiles)
		{
			int length = lstrlenW(path);
			PWCHAR search = new WCHAR[length + 3];
			StrCpyW(search, path);

			if (path[length - 1] != L'\\') StrCatW(search, L"\\");
			StrCatW(search, L"*");

			WIN32_FIND_DATAW data;
			HANDLE find = FindFirstFileW(search, &data);
			delete search;

			if (find != INVALID_HANDLE_VALUE)
			{
				Array<LPWSTR> *files = new Array<LPWSTR>();

				do
				{
					if (StrCmpW(data.cFileName, L".") && StrCmpW(data.cFileName, L".."))
					{
						if (((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) ^ getFiles)
						{
							PWCHAR name = new WCHAR[lstrlenW(data.cFileName) + 1];
							StrCpyW(name, data.cFileName);

							files->Add(name);
						}
					}
				}
				while (FindNextFileW(find, &data));
				FindClose(find);

				return files;
			}

			return NULL;
		}

		BOOL Exists(LPCWSTR path)
		{
			DWORD attributes = GetFileAttributesW(path);
			return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
		}
		Array<LPWSTR>* GetDirectories(LPCWSTR path)
		{
			return __GetFilesOrDirectories(path, FALSE);
		}
		Array<LPWSTR>* GetFiles(LPCWSTR path)
		{
			return __GetFilesOrDirectories(path, TRUE);
		}
	}

	namespace File
	{
		BOOL Exists(LPCWSTR path)
		{
			DWORD attributes = GetFileAttributesW(path);
			return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY;
		}
		LPBYTE Read(LPCWSTR path, LPDWORD bytesRead)
		{
			HANDLE file = CreateFileW(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file == INVALID_HANDLE_VALUE) return NULL;

			LPBYTE result = NULL;

			DWORD size = GetFileSize(file, NULL);
			if (size != INVALID_FILE_SIZE)
			{
				result = new BYTE[size];

				if (ReadFile(file, result, size, NULL, NULL))
				{
					*bytesRead = size;
				}
				else
				{
					delete result;
					result = NULL;
				}
			}

			CloseHandle(file);
			return result;
		}
		BOOL Write(LPCWSTR path, LPBYTE data, DWORD length)
		{
			HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file == INVALID_HANDLE_VALUE) return NULL;

			BOOL result = WriteFile(file, data, length, NULL, NULL);
			CloseHandle(file);

			return result;
		}
	}

	namespace Registry
	{
		HKEY __OpenKey(HKEY hive, LPCWSTR subkey, REGSAM desiredAccess)
		{
			HKEY key;
			if (RegOpenKeyExW(hive, subkey, 0, desiredAccess, &key) != ERROR_SUCCESS) return NULL;

			return key;
		}
		BOOL __QueryKey(HKEY hive, LPDWORD subKeyCount, LPDWORD valueCount, LPDWORD maxKeyNameLength, LPDWORD maxValueNameLength)
		{
			WCHAR achClass[MAX_PATH] = L"";
			DWORD classNameSize = MAX_PATH;
			DWORD maxClassSize;
			DWORD maxValueLength;
			DWORD securityDescriptor;
			FILETIME lastWriteTime;

			return RegQueryInfoKeyW(hive, achClass, &classNameSize, NULL, subKeyCount, maxKeyNameLength, &maxClassSize, valueCount, maxValueNameLength, &maxValueLength, &securityDescriptor, &lastWriteTime) == ERROR_SUCCESS;
		}

		LPWSTR GetValueString(HKEY hive, LPCWSTR subkey, LPCWSTR name)
		{
			HKEY key = __OpenKey(hive, subkey, KEY_QUERY_VALUE);
			if (!key) return NULL;

			LPWSTR result = NULL;

			WCHAR buffer[1024];
			DWORD size = sizeof(buffer);
			LSTATUS status = RegQueryValueExW(key, name, 0, NULL, (LPBYTE)buffer, &size);

			if (status == ERROR_SUCCESS)
			{
				result = new WCHAR[(size - 1) / 2 + 1];
				StrCpyW(result, buffer);
			}

			RegCloseKey(key);
			return result;
		}
		BOOL GetValueDword(HKEY hive, LPCWSTR subkey, LPCWSTR name, PDWORD value)
		{
			HKEY key = __OpenKey(hive, subkey, KEY_QUERY_VALUE);
			if (!key) return FALSE;

			DWORD size = sizeof(DWORD);
			BOOL result = RegQueryValueExW(key, name, 0, NULL, (LPBYTE)value, &size) == ERROR_SUCCESS;

			RegCloseKey(key);
			return result;
		}
		BOOL SetValueString(HKEY hive, LPCWSTR subkey, LPCWSTR name, LPCWSTR value, BOOL isExpandedString)
		{
			HKEY key = __OpenKey(hive, subkey, KEY_ALL_ACCESS);
			if (!key) return FALSE;

			BOOL result = RegSetValueExW(key, name, 0, isExpandedString ? REG_EXPAND_SZ : REG_SZ, (LPBYTE)value, (lstrlenW(value) + 1) * sizeof(WCHAR)) == ERROR_SUCCESS;

			RegCloseKey(key);
			return result;
		}
		BOOL SetValueDword(HKEY hive, LPCWSTR subkey, LPCWSTR name, DWORD value)
		{
			HKEY key = __OpenKey(hive, subkey, KEY_ALL_ACCESS);
			if (!key) return FALSE;

			BOOL result = RegSetValueExW(key, name, 0, REG_DWORD, (LPBYTE)&value, sizeof(value)) == ERROR_SUCCESS;

			RegCloseKey(key);
			return result;
		}
		BOOL DeleteValue(HKEY hive, LPCWSTR subkey, LPCWSTR name)
		{
			HKEY key = __OpenKey(hive, subkey, KEY_ALL_ACCESS);
			if (!key) return FALSE;

			BOOL result = RegDeleteValueW(key, name) == ERROR_SUCCESS;

			RegCloseKey(key);
			return result;
		}

		BOOL CreateKey(HKEY hive, LPCWSTR subkey, LPCWSTR name)
		{
			HKEY key = __OpenKey(hive, subkey, KEY_ALL_ACCESS);
			if (!key) return FALSE;

			HKEY createdKey;
			RegCreateKeyW(key, name, &createdKey);
			BOOL result = createdKey != NULL;

			RegCloseKey(key);
			RegCloseKey(createdKey);
			return result;
		}
		BOOL DeleteKey(HKEY hive, LPCWSTR subkey, LPCWSTR name)
		{
			HKEY key = __OpenKey(hive, subkey, KEY_ALL_ACCESS);
			if (!key) return FALSE;

			BOOL result = RegDeleteKeyW(key, name) == ERROR_SUCCESS;

			RegCloseKey(key);
			return result;
		}
		Array<LPWSTR>* GetKeyNames(HKEY hive, LPCWSTR subkey)
		{
			Array<LPWSTR> *result = NULL;

			if (hive && subkey)
			{
				HKEY key = __OpenKey(hive, subkey, KEY_READ);
				if (key)
				{
					DWORD subKeyCount;
					DWORD valueCount;
					DWORD maxKeyNameLength;
					DWORD maxValueNameLength;

					if (__QueryKey(key, &subKeyCount, &valueCount, &maxKeyNameLength, &maxValueNameLength))
					{
						PWCHAR name = new WCHAR[maxKeyNameLength + 1];
						result = new Array<LPWSTR>(subKeyCount);
						int resultIndex = 0;

						for (DWORD i = 0; i < subKeyCount; i++)
						{
							DWORD nameSize = maxKeyNameLength + 1;
							name[0] = L'\0';

							if (RegEnumKeyExW(key, i, name, &nameSize, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) continue;

							PWCHAR buffer = new WCHAR[nameSize + 1];
							StrCpyW(buffer, name);
							result->Add(buffer);
						}

						delete name;
					}

					RegCloseKey(key);
				}
			}

			return result;
		}
		Array<LPWSTR>* GetValueNames(HKEY hive, LPCWSTR subkey)
		{
			Array<LPWSTR> *result = NULL;

			if (hive && subkey)
			{
				HKEY key = __OpenKey(hive, subkey, KEY_READ);
				if (key)
				{
					DWORD subKeyCount;
					DWORD valueCount;
					DWORD maxKeyNameLength;
					DWORD maxValueNameLength;

					if (__QueryKey(key, &subKeyCount, &valueCount, &maxKeyNameLength, &maxValueNameLength))
					{
						PWCHAR name = new WCHAR[maxValueNameLength + 1];
						result = new Array<LPWSTR>(valueCount);
						int resultIndex = 0;

						for (DWORD i = 0; i < valueCount; i++)
						{
							DWORD nameSize = maxValueNameLength + 1;
							name[0] = L'\0';

							if (RegEnumValueW(key, i, name, &nameSize, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) continue;
							if (RegQueryValueExW(key, name, 0, NULL, NULL, NULL) != ERROR_SUCCESS) continue;

							PWCHAR buffer = new WCHAR[nameSize + 1];
							StrCpyW(buffer, name);
							result->Add(buffer);
						}

						delete name;
					}

					RegCloseKey(key);
				}
			}

			return result;
		}
	}

	namespace Process
	{
		typedef NTSTATUS(NTAPI *__NtQueryInformationProcess)(HANDLE ProcessHandle, DWORD ProcessInformationClass, PVOID ProcessInformation, DWORD ProcessInformationLength, PDWORD ReturnLength);

		//FEATURE: Other privileges than SE_DEBUG_NAME
		//TODO: Test if it works
		//TODO: Bug: Returns true even if failed
		/*BOOL EnableDebugPrivilege()
		{
			BOOL result = FALSE;
			HANDLE token = NULL;
			HANDLE process = GetCurrentProcess();

			if (process && OpenProcessToken(process, TOKEN_ADJUST_PRIVILEGES, &token))
			{
				LUID luid;
				if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
				{
					TOKEN_PRIVILEGES tokenPrivileges;
					tokenPrivileges.PrivilegeCount = 1;
					tokenPrivileges.Privileges[0].Luid = luid;
					tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

					result = AdjustTokenPrivileges(token, FALSE, &tokenPrivileges, sizeof(tokenPrivileges), NULL, NULL);
				}
			}

			if (process) CloseHandle(process);
			if (token) CloseHandle(token);
			return result;
		}*/
		LPCWSTR GetIntegrityLevelName(DWORD integrityLevel)
		{
			if (integrityLevel >= SECURITY_MANDATORY_UNTRUSTED_RID && integrityLevel < SECURITY_MANDATORY_LOW_RID) return L"Untrusted";
			else if (integrityLevel >= SECURITY_MANDATORY_LOW_RID && integrityLevel < SECURITY_MANDATORY_MEDIUM_RID) return L"Low";
			else if (integrityLevel >= SECURITY_MANDATORY_MEDIUM_RID && integrityLevel < SECURITY_MANDATORY_HIGH_RID) return L"Medium";
			else if (integrityLevel >= SECURITY_MANDATORY_HIGH_RID && integrityLevel < SECURITY_MANDATORY_SYSTEM_RID) return L"High";
			else if (integrityLevel >= SECURITY_MANDATORY_SYSTEM_RID) return L"System";
			else return NULL;
		}

		DWORD GetProcessIdByName(LPCWSTR name)
		{
			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (snapshot == INVALID_HANDLE_VALUE) return 0;

			PROCESSENTRY32W process;
			ZeroMemory(&process, sizeof(process));
			process.dwSize = sizeof(process);

			DWORD processId = 0;
			if (Process32FirstW(snapshot, &process))
			{
				do
				{
					if (!StrCmpIW(process.szExeFile, name))
					{
						processId = process.th32ProcessID;
						break;
					}
				}
				while (Process32NextW(snapshot, &process));
			}

			CloseHandle(snapshot);
			return processId;
		}
		BOOL CreateProcessWithIntegrity(LPCWSTR commandLine, DWORD integrityLevel, LPDWORD processId)
		{
			BOOL result = FALSE;
			HANDLE process = GetCurrentProcess();
			HANDLE token = NULL;
			HANDLE newToken = NULL;
			SID_IDENTIFIER_AUTHORITY authority = SECURITY_MANDATORY_LABEL_AUTHORITY;
			PSID integritySid = NULL;
			TOKEN_MANDATORY_LABEL tokenMandatoryLabel = { 0 };
			STARTUPINFOW startupInfo = { sizeof(startupInfo) };
			PROCESS_INFORMATION processInformation = { 0 };

			if (OpenProcessToken(process, TOKEN_DUPLICATE | TOKEN_QUERY | TOKEN_ADJUST_DEFAULT | TOKEN_ASSIGN_PRIMARY, &token))
			{
				if (DuplicateTokenEx(token, 0, NULL, SecurityImpersonation, TokenPrimary, &newToken))
				{
					if (AllocateAndInitializeSid(&authority, 1, integrityLevel, 0, 0, 0, 0, 0, 0, 0, &integritySid))
					{
						tokenMandatoryLabel.Label.Attributes = SE_GROUP_INTEGRITY;
						tokenMandatoryLabel.Label.Sid = integritySid;

						if (SetTokenInformation(newToken, TokenIntegrityLevel, &tokenMandatoryLabel, (sizeof(tokenMandatoryLabel) + GetLengthSid(integritySid))))
						{
							WCHAR path[1024];
							StrCpyNW(path, commandLine, sizeof(path) / sizeof(WCHAR));

							result = CreateProcessAsUserW(newToken, NULL, path, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInformation);
						}
					}
				}
			}

			if (process) CloseHandle(process);
			if (token) CloseHandle(token);
			if (newToken) CloseHandle(newToken);
			if (integritySid) FreeSid(integritySid);
			if (processInformation.hProcess) CloseHandle(processInformation.hProcess);
			if (processInformation.hThread) CloseHandle(processInformation.hThread);

			*processId = processInformation.dwProcessId;
			return result;
		}

		LPWSTR GetProcessName(DWORD processId)
		{
			HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
			if (!process) return NULL;

			WCHAR buffer[MAX_PATH + 1];
			DWORD size = GetProcessImageFileNameW(process, buffer, sizeof(buffer) / sizeof(WCHAR));
			if (size == 0) return NULL;

			PWCHAR fileName = wcsrchr(buffer, L'\\');
			if (!fileName) return NULL;

			fileName++;
			PWCHAR result = new WCHAR[lstrlenW(fileName) + 1];
			StrCpyW(result, fileName);

			return result;
		}
		LPWSTR GetProcessCommandLine(DWORD processId)
		{
			HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
			LPWSTR result = NULL;

			HMODULE module = GetModuleHandleW(L"ntdll.dll");
			if (module)
			{
				LPVOID queryInformationProcess = (LPVOID)GetProcAddress(module, "NtQueryInformationProcess");
				if (queryInformationProcess)
				{
					PROCESS_BASIC_INFORMATION processBasicInformation;
					if (NT_SUCCESS(((__NtQueryInformationProcess)queryInformationProcess)(process, 0, &processBasicInformation, sizeof(processBasicInformation), NULL)))
					{
						PRTL_USER_PROCESS_PARAMETERS paramsAddress;
						if (ReadProcessMemory(process, &processBasicInformation.PebBaseAddress->ProcessParameters, &paramsAddress, sizeof(PVOID), NULL))
						{
							UNICODE_STRING commandLine;
							if (ReadProcessMemory(process, &(paramsAddress->CommandLine), &commandLine, sizeof(commandLine), NULL))
							{
								result = new WCHAR[commandLine.Length / 2 + 1];
								if (ReadProcessMemory(process, commandLine.Buffer, result, commandLine.Length, NULL))
								{
									result[commandLine.Length / 2] = L'\0';
								}
								else
								{
									delete result;
									result = NULL;
								}
							}
						}
					}
				}
			}

			CloseHandle(process);
			return result;
		}
		DWORD GetProcessIntegrityLevel(HANDLE process)
		{
			DWORD result = -1;
			HANDLE token;
			PTOKEN_MANDATORY_LABEL tokenMandatoryLabel = NULL;
			DWORD lengthNeeded;

			if (OpenProcessToken(process, TOKEN_QUERY, &token))
			{
				if (!GetTokenInformation(token, TOKEN_INFORMATION_CLASS::TokenIntegrityLevel, NULL, 0, &lengthNeeded))
				{
					if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
					{
						tokenMandatoryLabel = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0, lengthNeeded);
						if (tokenMandatoryLabel)
						{
							if (GetTokenInformation(token, TOKEN_INFORMATION_CLASS::TokenIntegrityLevel, tokenMandatoryLabel, lengthNeeded, &lengthNeeded))
							{
								result = *GetSidSubAuthority(tokenMandatoryLabel->Label.Sid, (DWORD)(UCHAR)(*GetSidSubAuthorityCount(tokenMandatoryLabel->Label.Sid) - 1));
							}
						}
					}
				}
			}

			if (token) CloseHandle(token);
			if (tokenMandatoryLabel) LocalFree(tokenMandatoryLabel);
			return result;
		}
		DWORD GetParentProcessId(DWORD processId)
		{
			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (snapshot == INVALID_HANDLE_VALUE) return 0;

			PROCESSENTRY32 process;
			ZeroMemory(&process, sizeof(process));
			process.dwSize = sizeof(process);

			DWORD parentProcessId = 0;
			if (Process32First(snapshot, &process))
			{
				do
				{
					if (process.th32ProcessID == processId)
					{
						parentProcessId = process.th32ParentProcessID;
						break;
					}
				}
				while (Process32Next(snapshot, &process));
			}

			CloseHandle(snapshot);
			return parentProcessId;
		}
		Array<HWND>* GetProcessWindows(DWORD processID)
		{
			Array<HWND> *windows = new Array<HWND>();
			HWND hwnd = NULL;

			do
			{
				hwnd = FindWindowExW(NULL, hwnd, NULL, NULL);

				DWORD windowProcessId = 0;
				GetWindowThreadProcessId(hwnd, &windowProcessId);

				if (windowProcessId == processID) windows->Add(hwnd);
			}
			while (hwnd != NULL);

			return windows;
		}
		BOOL InjectDll(HANDLE process, LPCWSTR dllPath)
		{
			HMODULE module = GetModuleHandleW(L"kernel32.dll");
			if (!module) return FALSE;

			LPVOID loadLibraryAddress = (LPVOID)GetProcAddress(module, "LoadLibraryW");
			if (!loadLibraryAddress) return FALSE;

			LPVOID allocatedMemoryAddress = VirtualAllocEx(process, NULL, lstrlenW(dllPath) * sizeof(WCHAR), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (!allocatedMemoryAddress) return FALSE;

			if (!WriteProcessMemory(process, allocatedMemoryAddress, dllPath, lstrlenW(dllPath) * sizeof(WCHAR), NULL)) return FALSE;
			if (!CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibraryAddress, allocatedMemoryAddress, 0, NULL)) return FALSE;

			return TRUE;
		}
	}

	namespace Service
	{
		SC_HANDLE GetServiceByName(LPCWSTR name)
		{
			SC_HANDLE serviceManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (!serviceManager) return NULL;

			SC_HANDLE service = OpenServiceW(serviceManager, name, SC_MANAGER_ALL_ACCESS);

			CloseServiceHandle(serviceManager);
			return service;
		}
		DWORD GetServiceState(SC_HANDLE service)
		{
			SERVICE_STATUS_PROCESS status;
			DWORD bytesNeeded;
			if (!QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) return 0;

			return status.dwCurrentState;
		}
		DWORD GetServiceProcessId(SC_HANDLE service)
		{
			SERVICE_STATUS_PROCESS status;
			DWORD bytesNeeded;
			if (!QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) return 0;

			return status.dwProcessId;
		}
		BOOL StartServiceWait(SC_HANDLE service, DWORD expectedState, DWORD delayMilliseconds, DWORD timeoutMilliseconds)
		{
			BOOL result = FALSE;
			DWORD startTime = GetTickCount();

			while (!result && GetTickCount() - startTime < timeoutMilliseconds)
			{
				StartServiceW(service, 0, NULL);
				Sleep(delayMilliseconds);

				result = GetServiceState(service) == expectedState;
			}

			return result;
		}
		BOOL ControlServiceWait(SC_HANDLE service, DWORD control, DWORD expectedState, DWORD delayMilliseconds, DWORD timeoutMilliseconds)
		{
			BOOL result = FALSE;
			DWORD startTime = GetTickCount();
			SERVICE_STATUS_PROCESS status;

			while (!result && GetTickCount() - startTime < timeoutMilliseconds)
			{
				ControlService(service, control, (LPSERVICE_STATUS)&status);
				Sleep(delayMilliseconds);

				result = GetServiceState(service) == expectedState;
			}

			return result;
		}
	}

	namespace FileOperation
	{
		BOOL __FileOperation(LPCWSTR srcPath, LPCWSTR destPath, DWORD action)
		{
			BOOL result = FALSE;
			IFileOperation *fileOperation = NULL;
			IShellItem *shellItemFrom = NULL;
			IShellItem *shellItemTo = NULL;

			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr))
			{
				hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(&fileOperation));

				if (SUCCEEDED(hr))
				{
					hr = fileOperation->SetOperationFlags(FOF_NOCONFIRMATION | FOF_SILENT | FOFX_SHOWELEVATIONPROMPT | FOFX_NOCOPYHOOKS | FOFX_REQUIREELEVATION | FOF_NOERRORUI);
					if (SUCCEEDED(hr))
					{
						hr = SHCreateItemFromParsingName(srcPath, NULL, IID_PPV_ARGS(&shellItemFrom));
						if (SUCCEEDED(hr))
						{
							if (destPath)
							{
								LPWSTR destDirectory = Path::GetDirectoryName(destPath);
								hr = SHCreateItemFromParsingName(destDirectory, NULL, IID_PPV_ARGS(&shellItemTo));
								delete destDirectory;
							}

							if (SUCCEEDED(hr))
							{
								LPWSTR newFileName = destPath ? Path::GetFileName(destPath) : NULL;

								switch (action)
								{
									case 1:
										hr = fileOperation->CopyItem(shellItemFrom, shellItemTo, newFileName, NULL);
										if (SUCCEEDED(hr)) result = TRUE;
										break;
									case 2:
										hr = fileOperation->MoveItem(shellItemFrom, shellItemTo, newFileName, NULL);
										if (SUCCEEDED(hr)) result = TRUE;
										break;
									case 3:
										hr = fileOperation->DeleteItem(shellItemFrom, NULL);
										if (SUCCEEDED(hr)) result = TRUE;
										break;
								}

								if (newFileName) delete newFileName;
								if (shellItemTo) shellItemTo->Release();
							}

							shellItemFrom->Release();
						}

						if (SUCCEEDED(hr)) hr = fileOperation->PerformOperations();
					}

					fileOperation->Release();
				}
				CoUninitialize();
			}

			return result;
		}

		BOOL Copy(LPCWSTR srcPath, LPCWSTR destPath)
		{
			return __FileOperation(srcPath, destPath, 1);
		}
		BOOL Move(LPCWSTR srcPath, LPCWSTR destPath)
		{
			return __FileOperation(srcPath, destPath, 2);
		}
		BOOL Delete(LPCWSTR path)
		{
			return __FileOperation(path, NULL, 3);
		}
	}

	namespace Message
	{
		void Information(LPCWSTR message)
		{
			Information(L"Information", message);
		}
		void Information(LPCWSTR title, LPCWSTR message)
		{
			MessageBoxW(NULL, message, title, MB_OK | MB_ICONASTERISK);
		}
		void Warning(LPCWSTR message)
		{
			Warning(L"Warning", message);
		}
		void Warning(LPCWSTR title, LPCWSTR message)
		{
			MessageBoxW(NULL, message, title, MB_OK | MB_ICONWARNING);
		}
		void Error(LPCWSTR message, BOOL exitProcess)
		{
			Error(L"Error", message, exitProcess);
		}
		void Error(LPCWSTR title, LPCWSTR message, BOOL exitProcess)
		{
			MessageBoxW(NULL, message, title, MB_OK | MB_ICONERROR);
			if (exitProcess) ExitProcess(0);
		}
		BOOL Confirmation(LPCWSTR message, BOOL warning)
		{
			return Confirmation(L"Confirmation", message, false);
		}
		BOOL Confirmation(LPCWSTR title, LPCWSTR message, BOOL warning)
		{
			return MessageBoxW(NULL, message, title, MB_OKCANCEL | (warning ? MB_ICONWARNING : MB_ICONASTERISK)) == IDOK;
		}
	}
}