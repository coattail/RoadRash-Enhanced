#include <windows.h>
#include "versionhelpers.h"

typedef NTSTATUS(WINAPI* RTLVERIFYVERSIONINFOPROC)(PRTL_OSVERSIONINFOEXW, ULONG, ULONGLONG);
typedef ULONGLONG(WINAPI* VERSETCONDITIONMASKPROC)(ULONGLONG, DWORD, BYTE);
typedef const char* (CDECL* WINE_GET_VERSIONPROC)();
typedef void (CDECL* WINE_GET_HOST_VERSIONPROC)(const char** sysname, const char** release);

static RTLVERIFYVERSIONINFOPROC RtlVerifyVersionInfo;
static VERSETCONDITIONMASKPROC VerSetConditionMaskProc;
static WINE_GET_VERSIONPROC wine_get_version;
static WINE_GET_HOST_VERSIONPROC wine_get_host_version;

/* GetProcAddress is rather slow so we use a function to initialize it once on startup */
void verhelp_init()
{
    HMODULE mod = GetModuleHandleA("ntdll.dll");
    if (mod)
    {
        RtlVerifyVersionInfo = (RTLVERIFYVERSIONINFOPROC)GetProcAddress(mod, "RtlVerifyVersionInfo");
        wine_get_version = (WINE_GET_VERSIONPROC)GetProcAddress(mod, "wine_get_version");
        wine_get_host_version = (WINE_GET_HOST_VERSIONPROC)GetProcAddress(mod, "wine_get_host_version");
    }

    mod = GetModuleHandleA("Kernel32.dll");
    if (mod)
    {
        VerSetConditionMaskProc = (VERSETCONDITIONMASKPROC)GetProcAddress(mod, "VerSetConditionMask");
    }
}

BOOL verhelp_verify_version(PRTL_OSVERSIONINFOEXW versionInfo, ULONG typeMask, ULONGLONG conditionMask)
{
    return RtlVerifyVersionInfo ?
        RtlVerifyVersionInfo(versionInfo, typeMask, conditionMask) == 0 :
        VerifyVersionInfoW(versionInfo, typeMask, conditionMask);
}

ULONGLONG verhelp_set_mask(ULONGLONG ConditionMask, DWORD TypeMask, BYTE Condition)
{
    return VerSetConditionMaskProc ? VerSetConditionMaskProc(ConditionMask, TypeMask, Condition) : 0;
}

const char* verhelp_wine_get_version()
{
    return wine_get_version ? wine_get_version() : NULL;
}

void verhelp_wine_get_host_version(const char** sysname, const char** release)
{
    if (wine_get_host_version)
    {
        wine_get_host_version(sysname, release);
        return;
    }

    if (sysname)
        *sysname = NULL;

    if (release)
        *release = NULL;
}
