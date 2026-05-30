#TOOLCHAIN="$HOME/Workspace/VSCODE/llvm-mingw-20260519-msvcrt-ubuntu-22.04-x86_64"
TOOLCHAIN="${TOOLCHAIN_PATH:-$HOME/Workspace/VSCODE/llvm-mingw-20260519-msvcrt-ubuntu-22.04-x86_64}"
PROJECT=$(cd "$(dirname "$0")" && pwd)
TARGET="x86_64-w64-mingw32"
cmake -B "build_windows/$TARGET" \
        -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN/toolchain.cmake"\
        -DTOOLCHAIN_PATH="$TOOLCHAIN" \
        -DTARGET="$TARGET"
cmake --build "build_windows/$TARGET"

