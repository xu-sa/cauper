#!/bin/bash

set -e
ABI="${1:-arm64-v8a}"
API="21"
PROJECT=$(cd "$(dirname "$0")" && pwd)
BUILD_DIR="$PROJECT/build-android/$ABI"

# Override NDK via env var, or fallback
NDK_PATH="${NDK_PATH:-$HOME/Workspace/Android/SDK/ndk/27.0.12077973}"

cmake -B "$BUILD_DIR" \
    -DCMAKE_TOOLCHAIN_FILE="$NDK_PATH/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI="$ABI" \
    -DANDROID_PLATFORM="android-$API" \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_PREBUILT_DIR="$PROJECT/android_prebuilt/$ABI" \
    -DPLATFORM_ANDROID=ON

cmake --build "$BUILD_DIR" -j$(nproc)

echo "✅ Built for $ABI → $BUILD_DIR/lib/"

