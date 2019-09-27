# WinAPIEx

*Extensions to WinAPI.*

**WinAPIEx.h** is the file you include. In addition to WinAPI, WinAPIEx has
functions to simplify boilerplate code for repeating tasks, such as
file/registry/process operations. The target audience is pentesters and
developers who write C++ code that interacts with the operating system
(Windows).

## Minimalism

### One .h and one .cpp file - for a reason

In pentesting scenarios, single binaries with no additional dependencies become
handy. Deployment issues that manifest in undefined behavior can be avoided.

### No classes & methods - just functions</h3>

Typically, a class wraps up an entire entity and all of its features, like the
`Process` class in .NET Framework.

However, having a class that is not 100 % feature complete, means exporting the
HANDLE, procceeding with WinAPI, and then going back to the class object
instance. This ambiguousness can be avoided by either extending WinAPI as it is,
or by wrapping up everything. The latter case is covered by frameworks, such as
Qt or Boost.

In WinAPIEx, singular functions, like `DWORD GetProcessIdByName(LPCWSTR)` fit
perfectly into the existing WinAPI function set. Existing code does not need to
be migrated.

## API Structure

### Parameters and return values

In WinAPI, most commonly a function returns a `BOOL` value indicating
success/failure with a parameter to an existing buffer. This way, buffers can be
allocated on the stack and reused. For performance critical situations, less
abstraction is nice. However, WinAPIEx is a little more abstract to become
handy.

Examples:

* A function that returns a string, returns `LPWSTR`, or `NULL` if it failed. This buffer has to be deleted after calling the function.
* A function that returns a `DWORD` (or anything that is not a buffer), will either return 0, or -1 on failure, depending on the individual case. Or the function returns `BOOL` and accepts a `LPDWORD` parameter. Again, this is up to the individual case.

### Function names & structure

WinAPI is flat when it comes to function names. The initial intent to design
WinAPIEx this way is not especially comprehensive. That's why WinAPIEx has
namespaces to separate functions a bit. The root namespace is `C`. So by
applying `using namespace C;` or `using namespace C::Environment;` there is
quicker access to these functions.

### Classes

No classes. There is just one struct, `Array<T>`. This is the most simplistic
way to return an array of values, or `NULL`. And no further abstraction
(getters/setters, etc.)

And no enums, to avoid casting back and forth between WinAPI const's and
WinAPIEx enums. Otherwise, usage would be mixed and ambiguous. If you are
looking for a framework instead, try Qt or Boost.

### Unicode vs. ANSI

`LPWSTR`. No ANSI functions.

## Precompiled Binaries

A few EXE and DLL files are included to become handy. All precompiled with `/MT`
and in both x86 and x64.

## PayloadExe.exe

Simple MessageBox that displays information about the executed file. Especially
the IL indicator is helpful when testing privilege escalation exploits.

[![](https://bytecode77.com/images/sites/framework/winapi-ex/payloadexe.png)](https://bytecode77.com/images/sites/framework/winapi-ex/payloadexe.png)

## PayloadDll.dll

A DLL, equal to PayloadExe.exe. Instead of displaying a MessageBox, the result
is stored into the registry, where it can be later retrieved. An entry to
`HKCU\Software\__PAYLOAD_DLL` is made each time the DLL was loaded.
Additionally, write access to `HKLM\Software\__PAYLOAD_DLL` is attempted.

[![](https://bytecode77.com/images/sites/framework/winapi-ex/payloaddll.png)](https://bytecode77.com/images/sites/framework/winapi-ex/payloaddll.png)

## Inject.exe

Simple injector. Arguments are Process ID and path to a DLL.

```
Inject.exe 1234 "C:\MyPayload.dll"
```

## CreateSandbox.exe

Starts a process with low IL (SECURITY_MANDATORY_LOW_RID). The commandline is
specified in the first argument of CreateSandbox.exe. Otherwise, cmd.exe is
started with low IL and in a different color.

*Tip: Pass PayloadExe.exe as commandline argument to CreateSandbox.exe, for
example by using drag&drop. You will see a MessageBox indicating low IL.*

## Downloads

Compiled binaries for:

![](https://bytecode77.com/images/shared/fileicons/exe.png) PayloadExe.exe<br />
![](https://bytecode77.com/images/shared/fileicons/dll.png) PayloadDll.dll<br />
![](https://bytecode77.com/images/shared/fileicons/exe.png) Inject.exe<br />
![](https://bytecode77.com/images/shared/fileicons/exe.png) CreateSandbox.exe

[![](https://bytecode77.com/images/shared/fileicons/zip.png) WinAPIEx 0.8.4 Binaries.zip](https://bytecode77.com/downloads/framework/winapi-ex/WinAPIEx%200.8.4%20Binaries.zip)

## Project Page

[![](https://bytecode77.com/images/shared/favicon16.png) bytecode77.com/framework/winapi-ex](https://bytecode77.com/framework/winapi-ex)