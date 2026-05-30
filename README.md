# cauper — ScreenCast & Control your PC via LAN using Android

Stream your PC screen to your Android device and control it over a local network — no internet required, zero cloud dependency.

---

## Project Structure

This repository contains **three sub-projects**, each in its own folder:

| Folder | Description | Language | Platform |
|--------|-------------|----------|----------|
| `capureclt/` | Client library (JNI layer for Android) | C/C++ | Linux, Android (NDK) |
| `capureser/` | Server (runs on PC, captures screen & handles input) | C/C++ | Linux, Windows |
| `cauperandroid/` | Android app (UI & integration) | Kotlin/Java | Android |

```
cauper/
├── lib/                  # Compressed third-party library archives
│   ├── capureclt_libs.tar.gz
│   └── capureser_libs.tar.gz
├── setup_libs.py         # Decompress libs into correct locations
├── capureclt/            # Client library
│   ├── build_android.sh  # Build for Android (NDK)
│   ├── build.sh          # Build for Linux
│   └── CMakeLists.txt
├── capureser/            # Server application
│   ├── src/
│   └── build scripts
├── cauperandroid/        # Android application
│   └── app/
└── README.md
```

---

## Prerequisites

### For building the **Android app** (`cauperandroid`)
- Android Studio (latest stable)
- Android NDK **r27+** (set via `$NDK_PATH` or in `local.properties`)
- CMake 3.20+

### For building **Server** (`capureser`)
- Linux: GCC/Clang, CMake, pkg-config
- Windows: MSVC / MinGW cross-compiler toolchain

### For building **Client lib** (`capureclt`)
- Linux host (or WSL2)
- Android NDK (for Android target)

---

## Getting Started

### Clone & prepare libraries

```bash
git clone <repo-url> cauper
cd cauper

# Decompress third-party libraries into correct locations
python3 setup_libs.py
```

This extracts:
- `lib/capureclt_libs.tar.gz` → `capureclt/lib/` (ffmpeg, etc.)
- `lib/capureser_libs.tar.gz` → `capureser/lib/` (ffmpeg, libportal, glib, pipewire, etc.)

### Build the Server (`capureser`)

```bash
cd capureser
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Build the Client library (`capureclt`) for Android

```bash
cd capureclt
export NDK_PATH=/path/to/your/ndk
./build_android.sh               # defaults to arm64-v8a
./build_android.sh armeabi-v7a   # for 32-bit devices
```

Output goes to `build-android/<abi>/lib/`.

### Build the Android app (`cauperandroid`)

1. Open `cauperandroid/` in Android Studio
2. Copy the built `.so` files from `capureclt/build-android/arm64-v8a/lib/` into `cauperandroid/app/src/main/jniLibs/arm64-v8a/`
3. Sync Gradle and run on device

> The app communicates with the server via **LAN** — both devices must be on the same network.

---

## Environment Variables

| Variable | Default | Used By |
|----------|---------|---------|
| `NDK_PATH` | `$HOME/Workspace/Android/SDK/ndk/27.0.12077973` | `build_android.sh` |
| `ANDROID_PREBUILT_DIR` | `./android_prebuilt/<abi>/` | CMake (override for custom prebuilt paths) |

---

## How Third-Party Libraries Work

To keep the repo lightweight, prebuilt third-party libs (ffmpeg, glib, pipewire, libportal) are stored as compressed archives under `lib/`.

- **`python3 setup_libs.py`** — decompresses them into each project's `lib/` folder
- If you need to **re-pack** after updating, use:
  ```bash
  # inside each project folder
  cd capureclt && tar -czf ../lib/capureclt_libs.tar.gz lib/
  cd capureser && tar -czf ../lib/capureser_libs.tar.gz lib/
  ```

---

## How It Works

```
┌─────────────────────────┐         LAN          ┌─────────────────────┐
│  Android Device         │   ◄─── WebSocket ──►  │  PC (Server)        │
│  cauperandroid app      │                       │  capureser           │
│  ┌───────────────────┐  │                       │  - screen capture    │
│  │ JNI (capureclt)   │──┤                       │  - input relay       │
│  │ - decode video    │  │                       │  - audio streaming   │
│  │ - network client  │  │                       └─────────────────────┘
│  └───────────────────┘  │
└─────────────────────────┘
```

- **Server** captures screen (via PipeWire on Linux / DXGI on Windows), encodes as H.264/H.265, and streams over WebSocket
- **Client lib** (C/C++ via JNI) decodes video and handles network I/O
- **Android app** renders the decoded frames onto a SurfaceView and forwards touch/input events back to the server

---

## Development Notes

- **Cross-compiling for Windows?** Use a mingw-w64 toolchain on Linux. See `capureser/` for CMake presets.
- **Adding a new third-party lib?** Build it separately, place headers + `.so`/`.a` files in the respective `lib/<platform>/` dir, then re-pack the archive.

---

## License

MIT — see [LICENSE](LICENSE) for details.
