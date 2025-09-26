# pulqui-limiter.vst3

experimental VST3 audio limiter based on https://github.com/Lucarda/pd-pulqui.

( since v0.1.2 there are 2, 3 and 4 band crossovers plugins added )

there might be binaries on https://github.com/Lucarda/pulqui-limiter.vst3/releases or compile yourself.


### additional notes on macOS

the plugins bundle is automatically built in github's CI and is not signed
with an apple-developer id. to use the plugin downloaded from the releases
section you must remove its quarantine attributes:

- open Applications/Utilities/Terminal
- type "sudo xattr -r -d com.apple.quarantine" and add a space
- drag the downloaded (and extracted) bundles onto Terminal to add the path
- hit enter
- enter your password.

now you can place the bundles in your vst3 default dir and the host will
find it and make it available. note that if you compile yourself you don't 
have to do all this quarantine stuff.

## Compiling

base dependencies:

- git
- Cmake
- C++ compiler (MSVC on Windows)

### dependencies for Linux:

    sudo apt-get install cmake gcc "libstdc++6" libx11-xcb-dev libxcb-util-dev libxcb-cursor-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libfontconfig1-dev libcairo2-dev libgtkmm-3.0-dev libsqlite3-dev libxcb-keysyms1-dev

#### clone repository:

    git clone https://github.com/Lucarda/pulqui-limiter.vst3.git

    cd pulqui-limiter.vst3

#### init submodules:

    git submodule update --init --recursive

#### make build folder and cd to it:

    mkdir build && cd build

linux:

    cmake ../ -DCMAKE_BUILD_TYPE:STRING=release

    make

win:

    cmake ../ -DCMAKE_BUILD_TYPE:STRING=release -DSMTG_USE_STATIC_CRT:BOOL=ON

    cmake --build . --config Release

mac:

    cmake -DCMAKE_BUILD_TYPE:STRING=release -DSMTG_DISABLE_CODE_SIGNING=on ../

    cmake --build . --config Release
   
![vst logo](VST_Compatible_Logo_Steinberg_with_TM.png)
