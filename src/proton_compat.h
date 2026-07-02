#pragma once

#include <windows.h>
#include <cstddef>

extern "C" bool CPVR_IsWine();
extern "C" bool CPVR_ProtonCompatEnabled();
extern "C" bool CPVR_ShouldEnableDred();
extern "C" bool CPVR_ShouldForceSteamVrRuntime();
extern "C" bool CPVR_DefaultDepthSubmit();
extern "C" bool CPVR_DefaultDlssMatrixHook();
extern "C" bool CPVR_ShouldInstallDlssPatternHooks();
extern "C" bool CPVR_ShouldEnableNvidiaInterop();
extern "C" HMODULE CPVR_LoadRealDxgi(char* outPath, size_t outPathSize);
