# pulqui-limiter.vst3

experimental VST3 audio limiter based on https://github.com/Lucarda/pd-pulqui.

there might be binaries on https://github.com/Lucarda/pulqui-limiter.vst3/releases or compile yourself.

## Compiling

basic Dependencies:

- git
- Cmake
- C++ compiler (MSVC on Windows)

### Linux

deps: `sudo apt-get install git cmake gcc "libstdc++6" libx11-xcb-dev libxcb-util-dev libxcb-cursor-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libfontconfig1-dev libcairo2-dev libgtkmm-3.0-dev libsqlite3-dev libxcb-keysyms1-dev`

```
mkdir build
cd build
cmake ../pulquilimiter-src -DCMAKE_BUILD_TYPE:STRING=debug
cmake --build .
```

### Windows

```
mkdir build
cd build
cmake ../pulquilimiter-src -DSMTG_USE_STATIC_CRT:BOOL=ON -DCMAKE_BUILD_TYPE:STRING=release
cmake --build .
```

### macOS

```
mkdir build
cd build
cmake -GXcode ../pulquilimiter-src
cmake --build .
```