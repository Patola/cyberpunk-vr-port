#!/usr/bin/env sh
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cd "$ROOT"

TARGETS="src CMakeLists.txt README.md docs/proton-compat.md mods"

echo "== Proton-sensitive Windows/runtime assumptions =="
rg -n \
  'GetSystemDirectoryA|System32|LoadLibraryA\("dxgi|LoadLibraryA\("d3d|XR_RUNTIME_JSON|steamxr_win64|RegGetValueA|Cyberpunk2077\.exe|CreateSharedHandle|OpenSharedHandle|DRED|XInput' \
  $TARGETS || true

echo
echo "== NVIDIA/CUDA/DLSS-specific paths =="
rg -n \
  'nvcuda|cudart|CUDA|NvOF|OpticalFlow|NVSDK_NGX|NGX|DLSS|sl\.interposer' \
  $TARGETS || true

echo
echo "== Proton compatibility controls =="
rg -n 'CPVR_|wine_get_version|WINEDLLOVERRIDES' $TARGETS scripts || true
