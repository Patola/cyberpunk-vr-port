#include "proton_compat.h"

#include <cstring>

namespace {

bool IsFalseValue(const char* value) {
    return _stricmp(value, "0") == 0 ||
        _stricmp(value, "false") == 0 ||
        _stricmp(value, "no") == 0 ||
        _stricmp(value, "off") == 0 ||
        _stricmp(value, "disable") == 0 ||
        _stricmp(value, "disabled") == 0;
}

bool IsTrueValue(const char* value) {
    return _stricmp(value, "1") == 0 ||
        _stricmp(value, "true") == 0 ||
        _stricmp(value, "yes") == 0 ||
        _stricmp(value, "on") == 0 ||
        _stricmp(value, "enable") == 0 ||
        _stricmp(value, "enabled") == 0;
}

bool ReadEnvBool(const char* name, bool defaultValue) {
    char value[64]{};
    const DWORD len = GetEnvironmentVariableA(name, value, static_cast<DWORD>(sizeof(value)));
    if (len == 0 || len >= sizeof(value)) {
        return defaultValue;
    }
    if (IsFalseValue(value)) {
        return false;
    }
    if (IsTrueValue(value)) {
        return true;
    }
    return defaultValue;
}

bool GetEnvString(const char* name, char* out, size_t outSize) {
    if (!out || outSize == 0) {
        return false;
    }
    out[0] = '\0';
    const DWORD len = GetEnvironmentVariableA(name, out, static_cast<DWORD>(outSize));
    if (len == 0 || len >= outSize) {
        out[0] = '\0';
        return false;
    }
    return out[0] != '\0';
}

bool SamePathCaseInsensitive(const char* a, const char* b) {
    if (!a || !b || !a[0] || !b[0]) {
        return false;
    }
    return _stricmp(a, b) == 0;
}

bool GetCurrentModulePath(char* out, size_t outSize) {
    if (!out || outSize == 0) {
        return false;
    }
    out[0] = '\0';
    HMODULE self = nullptr;
    if (!GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&CPVR_LoadRealDxgi),
            &self)) {
        return false;
    }
    const DWORD len = GetModuleFileNameA(self, out, static_cast<DWORD>(outSize));
    if (len == 0 || len >= outSize) {
        out[0] = '\0';
        return false;
    }
    return true;
}

HMODULE LoadDxgiCandidate(const char* path, char* outPath, size_t outPathSize) {
    if (!path || !path[0]) {
        return nullptr;
    }
    HMODULE module = LoadLibraryA(path);
    if (!module) {
        return nullptr;
    }

    char loadedPath[MAX_PATH]{};
    GetModuleFileNameA(module, loadedPath, MAX_PATH);

    char currentPath[MAX_PATH]{};
    if (GetCurrentModulePath(currentPath, sizeof(currentPath)) &&
        SamePathCaseInsensitive(currentPath, loadedPath)) {
        FreeLibrary(module);
        return nullptr;
    }

    if (outPath && outPathSize > 0) {
        strncpy_s(outPath, outPathSize, loadedPath[0] ? loadedPath : path, _TRUNCATE);
    }
    return module;
}

}

extern "C" bool CPVR_IsWine() {
    static int cached = -1;
    if (cached >= 0) {
        return cached != 0;
    }

    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    cached = (ntdll && GetProcAddress(ntdll, "wine_get_version")) ? 1 : 0;
    return cached != 0;
}

extern "C" bool CPVR_ProtonCompatEnabled() {
    char value[64]{};
    const DWORD len = GetEnvironmentVariableA("CPVR_PROTON_COMPAT", value, static_cast<DWORD>(sizeof(value)));
    if (len > 0 && len < sizeof(value)) {
        if (IsFalseValue(value)) {
            return false;
        }
        if (IsTrueValue(value) || _stricmp(value, "auto") == 0) {
            return _stricmp(value, "auto") == 0 ? CPVR_IsWine() : true;
        }
    }
    return CPVR_IsWine();
}

extern "C" bool CPVR_ShouldEnableDred() {
    return ReadEnvBool("CPVR_ENABLE_DRED", !CPVR_ProtonCompatEnabled());
}

extern "C" bool CPVR_ShouldForceSteamVrRuntime() {
    return ReadEnvBool("CPVR_FORCE_STEAMVR_RUNTIME", !CPVR_ProtonCompatEnabled());
}

extern "C" bool CPVR_DefaultAERSubmit() {
    return ReadEnvBool("CPVR_DEFAULT_AER_SUBMIT", !CPVR_ProtonCompatEnabled());
}

extern "C" bool CPVR_DefaultDepthSubmit() {
    return ReadEnvBool("CPVR_DEFAULT_DEPTH_SUBMIT", !CPVR_ProtonCompatEnabled());
}

extern "C" bool CPVR_DefaultDlssMatrixHook() {
    return ReadEnvBool("CPVR_DEFAULT_DLSS_MATRIX_HOOK", !CPVR_ProtonCompatEnabled());
}

extern "C" bool CPVR_ShouldInstallDlssPatternHooks() {
    return ReadEnvBool("CPVR_ENABLE_DLSS_PATTERN_HOOKS", !CPVR_ProtonCompatEnabled());
}

extern "C" bool CPVR_ShouldInstallSettingsResHook() {
    return ReadEnvBool("CPVR_ENABLE_SETTINGS_RES_HOOK", !CPVR_ProtonCompatEnabled());
}

extern "C" bool CPVR_ShouldEnableNvidiaInterop() {
    return ReadEnvBool("CPVR_ENABLE_NVIDIA_INTEROP", !CPVR_ProtonCompatEnabled());
}

extern "C" HMODULE CPVR_LoadRealDxgi(char* outPath, size_t outPathSize) {
    if (outPath && outPathSize > 0) {
        outPath[0] = '\0';
    }

    char overridePath[MAX_PATH]{};
    if (GetEnvString("CPVR_REAL_DXGI_PATH", overridePath, sizeof(overridePath))) {
        HMODULE module = LoadDxgiCandidate(overridePath, outPath, outPathSize);
        if (module) {
            return module;
        }
    }

    char systemPath[MAX_PATH]{};
    const UINT len = GetSystemDirectoryA(systemPath, MAX_PATH);
    if (len > 0 && len < MAX_PATH) {
        strcat_s(systemPath, "\\dxgi.dll");
        HMODULE module = LoadDxgiCandidate(systemPath, outPath, outPathSize);
        if (module) {
            return module;
        }
    }

    HMODULE module = LoadDxgiCandidate("dxgi.dll", outPath, outPathSize);
    return module;
}
