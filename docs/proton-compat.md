# Proton compatibility branch

This fork keeps Linux/Proton support as a small patch set on top of upstream.
The Windows mod remains a Windows DLL loaded inside Proton; the goal is to
cooperate with Proton's DXGI/D3D12/OpenXR stack instead of replacing it.

## Recommended first launch

Use a vanilla Cyberpunk 2077 install and start with conservative features:

```sh
WINEDLLOVERRIDES="dxgi=n,b" \
CPVR_PROTON_COMPAT=1 \
CPVR_ENABLE_DRED=0 \
CPVR_FORCE_STEAMVR_RUNTIME=0 \
CPVR_DEFAULT_AER_SUBMIT=0 \
CPVR_DEFAULT_DEPTH_SUBMIT=0 \
CPVR_ENABLE_NVIDIA_INTEROP=0 \
CPVR_ENABLE_SETTINGS_RES_HOOK=0 \
CPVR_ENABLE_DLSS_PATTERN_HOOKS=0 \
PROTON_LOG=1 \
%command%
```

`dxgi=n,b` is required so Wine loads this proxy from `bin/x64` before the
builtin/backend DXGI. The proxy then loads the real backend DXGI from the Proton
prefix/system directory and logs the resolved path in `cyberpunkvrport.log`.

For the tested dependency-mod install sequence and the extra DLL overrides used
by CET/RED4ext, see [Proton mod install notes](proton-mod-install.md).

## Current tested status

On Proton with WiVRn/OpenXR and an AMD RX 7900 XTX, the current known-good path
is mono OpenXR submit:

- `xr_mono_submit=1`
- `xr_aer_submit=0`
- `xr_aer_v2=0`
- `xr_mono_xqueue_wait=0`
- `xr_aer_xqueue_wait=0`
- `xr_mono_depth_capture=0`

The SettingsRes executable hook is disabled by default under Proton because it
reproduced a stale two-frame present loop while audio/input/game simulation kept
running. Classic non-CUDA AER remains experimental: on the tested Proton/WiVRn
setup it initialized but did not produce an initial valid left/right eye pair,
then entered a black/stalled startup path.

## Compatibility environment variables

- `CPVR_PROTON_COMPAT=auto|0|1`: defaults to `auto`, enabled when Wine is
  detected through `ntdll!wine_get_version`.
- `CPVR_REAL_DXGI_PATH=...`: explicitly chooses the backend `dxgi.dll` the proxy
  forwards to. Use only if the log shows the wrong backend.
- `CPVR_ENABLE_DRED=0|1`: defaults off under Proton, on under Windows.
- `CPVR_FORCE_STEAMVR_RUNTIME=0|1`: defaults off under Proton. Proton/wineopenxr
  should usually own OpenXR runtime selection.
- `CPVR_DEFAULT_AER_SUBMIT=0|1`: default value written to a new `vrport.ini`
  for `xr_aer_submit`; defaults off under Proton. Mono submit is the current
  known-good Proton path. Classic AER is still experimental because it can enter
  a black/stalled startup path before a valid alternating camera pair exists.
- `CPVR_DEFAULT_DEPTH_SUBMIT=0|1`: defaults off under Proton, preserving the
  upstream default under Windows.
- `CPVR_DEFAULT_DLSS_MATRIX_HOOK=0|1`: default value written to a new
  `vrport.ini` for `xr_dlss_matrix_hook`; defaults off under Proton.
- `CPVR_ENABLE_DLSS_PATTERN_HOOKS=0|1`: controls the older Cyberpunk executable
  DLSS resolution/matrix pattern patches. Defaults off under Proton because FSR
  users do not need these hooks and pattern patches are the riskiest class of
  compatibility issue.
- `CPVR_ENABLE_SETTINGS_RES_HOOK=0|1`: controls the executable hook that writes
  Cyberpunk's internal settings-resolution structure. Defaults off under Proton;
  on VKD3D-Proton it can leave the game presenting a stale two-frame loop while
  audio and simulation continue. Swapchain/window overrides remain active.
- `CPVR_ENABLE_NVIDIA_INTEROP=0|1`: controls NVIDIA-only AER V2/NvOF/CUDA style
  paths. Defaults off under Proton so an old `vrport.ini` with `xr_aer_v2=1`
  does not repeatedly try to initialize unavailable interop on AMD.
- `CPVR_DIAG_EXEC_HOOKS=...`: diagnostic override for executable patch hooks.
  If unset, normal policy is used. Set to `none`, `all`, or a separated list of
  `camera`, `projection`, `movement`, and `settings`. This is useful after an
  upstream rebase to isolate which hook family regressed Proton startup. The
  `settings` family still obeys `CPVR_ENABLE_SETTINGS_RES_HOOK`; set both
  variables only when intentionally retesting the known-risky SettingsRes hook.

## AMD / non-CUDA behavior

The Proton build is still a Windows DLL running inside Wine/Proton. ROCm/HIP is
not a drop-in replacement for the current CUDA path: the AER V2 pipeline imports
Windows D3D12 resources/fences into CUDA and uses NVIDIA Optical Flow. On an AMD
GPU, the expected first target is:

- launch and play through the non-CUDA AER/mono/OpenXR paths;
- keep `xr_aer_v2=0` and `CPVR_ENABLE_NVIDIA_INTEROP=0`;
- use Cyberpunk's native FSR setting rather than a DLSS-to-FSR wrapper;
- leave FSR Frame Generation off for the first VR tests because flat-screen
  frame interpolation can conflict with OpenXR frame pacing, late pose, and
  compositor reprojection.

An AMD-native quality path should be a later backend project. The clean shape is
an upscaler-neutral motion-vector/depth telemetry layer first, then an AMD or
shader-compute optical-flow/warp backend if needed.

## Linux cross-builds

There are two Linux-hosted build paths:

- MinGW is useful for a small `dxgi.dll` Proton smoke test.
- `msvc-wine` plus `clang-cl` is the recommended path for a complete build,
  because it keeps the MSVC C++ ABI required by RED4ext generated engine types.

### MinGW smoke test

The AMD/Proton-oriented MinGW build disables MSVC/NVIDIA-specific pieces that are
not expected to work on this path:

- RED4ext RTTI queries from the `dxgi.dll` proxy;
- NVIDIA Optical Flow D3D12 backend;
- CUDA/NvOF AER V2 backend, unless explicitly re-enabled.

Build command:

```sh
cmake -S . -B build-proton-compat -G Ninja \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
  -DAER_V2_NVOF=OFF

cmake --build build-proton-compat --target dxgi
```

The output DLL is:

```text
build-proton-compat/bin/dxgi.dll
```

`CyberpunkVR_Hands.dll` must not be built with GCC/MinGW for runtime testing. It
is a separate RED4ext plugin and uses RED4ext generated C++ engine layouts that
assume MSVC ABI class layout. GCC/MinGW reuses C++ base-class tail padding
differently; for example, RED4ext expects some derived fields after a `0x10`
base, while GCC places them at `0x0c`. Do not bypass those asserts for runtime
testing: a DLL that compiles with the wrong layouts is likely to crash or corrupt
state.

### msvc-wine clang-cl build

On Arch Linux, install `msvc-wine-git` from AUR. This provides the MSVC headers,
libraries, Windows SDK and CMake toolchain under `/opt/msvc`, while using native
Linux `clang-cl`/`lld-link` to produce Windows PE DLLs with the MSVC ABI.

Build both DLLs with:

```sh
./scripts/build-msvc-clang.sh
```

The output DLLs are:

```text
build-msvc-clang-dxgi/bin/dxgi.dll
build-msvc-clang-hands/CyberpunkVR_Hands.dll
```

The script defaults to `CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL` because the
current OpenXR loader dependency is also built for the dynamic MSVC runtime. The
resulting DLLs import `MSVCP140.dll`, `VCRUNTIME140.dll` and UCRT API DLLs. Under
Proton, install the usual Visual C++ 2015-2022 runtime into the game prefix if
those imports are not already satisfied by the game/mod stack.

The `msvc-wine` path has been checked with a small ABI probe: `clang-cl` places
derived fields after MSVC-style base-class tail padding, unlike GCC/MinGW. This
is why it is viable for `CyberpunkVR_Hands.dll`.

If static CRT linkage becomes important, revisit the OpenXR loader runtime
setting at the same time. Mixing `/MT` objects with an `/MD` OpenXR loader fails
the linker's runtime-library mismatch check.

## Rebase audit

After rebasing from upstream, run:

```sh
./scripts/proton-audit.sh
```

Review every hit for assumptions that can regress Proton support, especially:

- hardcoded `System32` DXGI/D3D12 loading;
- forced `steamxr_win64.json` / registry runtime discovery;
- DRED/debug D3D12 APIs;
- CUDA/NvOF shared-handle interop;
- DLSS/NGX executable pattern hooks;
- named `.exe` module anchors;
- XInput and Win32 window/input hooks.

Keep fixes in `src/proton_compat.*` whenever possible so this fork remains easy
to rebase.
