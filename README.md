# WinAPIEx

## Extensions to WinAPI

**WinAPIEx.h** is the only include file. WinAPIEx supplements WinAPI by providing additional functions, particularly simplifying boilerplate tasks, such as file/registry/process operations. The target audience is pentesters and developers who write C++ code that interacts with the operating system.

## Minimalism

### One .h/.cpp file

In pentesting scenarios, single binaries with no additional dependencies become handy. Deployment issues that manifest in undefined behavior can be avoided.

### No object model - just plain functions

Typically, a class wraps up an entire entity and all of its features, like the "File" class in .NET Framework.

However, having a class that is not feature complete, forces the developer to export the HANDLE, proceed with WinAPI, and then go back to the class object instance. This ambiguousness can be avoided by adopting to WinAPI as it is. An alternative is to use frameworks, such as Qt or Boost.

WinAPIEx is not a framework - it provides functions, like `DWORD GetProcessIdByName(LPCWSTR)`. They fit perfectly into the existing WinAPI function set. Existing code does not need to be migrated.

## API Structure

### Parameters and return values

In WinAPI, functions typically return a BOOL or error code and have a parameter pointing to a pre-allocated buffer. This is great for performance critical implementations. However, WinAPIEx is aimed to be a little more abstract.

Examples:

- A function that returns a string, returns LPWSTR, or NULL, if it failed. This buffer needs to be deleted after calling the function.
- A function that returns a DWORD, will either return 0, or -1 on failure, depending on the individual function. Or the function returns BOOL and accepts a LPDWORD parameter. Again, this is up to the individual function.

### Naming & structure

WinAPI provides functions with no hierarchy, other than namespaces. The root namespace is "C". By *using namespace*'s, the hierarchy can be flattened. The functions mix perfectly with existing WinAPI code.

### Classes

No classes. There is just one struct: `Array<T>`. This is the most simplistic way to return an array that includes a length descriptor.

And no enums, to avoid casting back and forth between WinAPI const's and WinAPIEx enums. Otherwise, usage would be mixed and ambiguous. If you are looking for a framework instead, try Qt or Boost.

### Unicode vs. ANSI

No ANSI functions. Happy new year, 1993.

## Precompiled Binaries

A set of useful EXE and DLL files are included. All precompiled with /MT and in both x86 and x64.

### PayloadExe.exe

Simple MessageBox that displays information about the executed file. Especially the IL indicator is helpful when testing privilege escalation exploits.

![](https://bytecode77.com/images/pages/winapi-ex/payloadexe.png)

### PayloadDll.dll

A DLL, equivalent to PayloadExe.exe. Instead of displaying a MessageBox, the result is stored in the registry. The key "HKCU\Software\__PAYLOAD_DLL" has a subkey for each successful DLL load. Additionally, write access to "HKLM\Software\__PAYLOAD_DLL" is attempted.

![](https://bytecode77.com/images/pages/winapi-ex/payloaddll.png)

### Inject.exe

Simple injector with 2 arguments: Process ID and path to a DLL.

```
Inject.exe 1234 "C:\MyPayload.dll"
```

### CreateSandbox.exe

Starts a sandboxed process (low IL). The commandline is specified in the first argument of CreateSandbox.exe. When omitted, cmd.exe is started with low IL and in a different color.

Example: Pass PayloadExe.exe as commandline argument to CreateSandbox.exe, e.g. by using drag&drop. You will see a MessageBox indicating low IL.

## Downloads

Compiled binaries for:

![](https://bytecode77.com/public/fileicons/exe.png) PayloadExe.exe<br />
![](https://bytecode77.com/public/fileicons/dll.png) PayloadDll.dll<br />
![](https://bytecode77.com/public/fileicons/exe.png) Inject.exe<br />
![](https://bytecode77.com/public/fileicons/exe.png) CreateSandbox.exe

[![](http://bytecode77.com/public/fileicons/zip.png) WinAPIEx 0.8.4.zip](https://bytecode77.com/downloads/WinAPIEx%200.8.4.zip)

## Project Page

[![](https://bytecode77.com/public/favicon16.png) bytecode77.com/winapi-ex](https://bytecode77.com/winapi-ex)