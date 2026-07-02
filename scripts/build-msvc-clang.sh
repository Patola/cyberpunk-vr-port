#!/usr/bin/env sh
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cd "$ROOT"

TOOLCHAIN_FILE="${CPVR_MSVC_TOOLCHAIN_FILE:-/opt/msvc/cmake/toolchain-x64-clang.cmake}"
DXGI_BUILD_DIR="${CPVR_MSVC_DXGI_BUILD_DIR:-build-msvc-clang-dxgi}"
HANDS_BUILD_DIR="${CPVR_MSVC_HANDS_BUILD_DIR:-build-msvc-clang-hands}"
MSVC_RUNTIME="${CPVR_MSVC_RUNTIME:-MultiThreadedDLL}"

if [ ! -f "$TOOLCHAIN_FILE" ]; then
    echo "msvc-wine clang-cl toolchain file not found: $TOOLCHAIN_FILE" >&2
    echo "Install msvc-wine-git or set CPVR_MSVC_TOOLCHAIN_FILE." >&2
    exit 1
fi

cmake -S . -B "$DXGI_BUILD_DIR" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_MSVC_RUNTIME_LIBRARY="$MSVC_RUNTIME" \
    -DAER_V2_NVOF=OFF \
    -DCPVR_ENABLE_NVOF_D3D12=OFF \
    -DCPVR_ENABLE_RED4EXT_PROXY_RTTI=OFF

cmake --build "$DXGI_BUILD_DIR" --target dxgi -j"$(nproc)"

cmake -S src/red4ext_plugin -B "$HANDS_BUILD_DIR" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_MSVC_RUNTIME_LIBRARY="$MSVC_RUNTIME"

cmake --build "$HANDS_BUILD_DIR" --target CyberpunkVR_Hands -j"$(nproc)"

echo
echo "Built:"
echo "  $DXGI_BUILD_DIR/bin/dxgi.dll"
echo "  $HANDS_BUILD_DIR/CyberpunkVR_Hands.dll"
