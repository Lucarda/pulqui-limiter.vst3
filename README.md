# pulqui-limiter.vst3

## Linux

```
mkdir build
cd build
cmake ../pulquilimiter-src -DCMAKE_BUILD_TYPE:STRING=debug
cmake --build .
```

## Windows

```
mkdir build
cd build
cmake ../pulquilimiter-src -DSMTG_USE_STATIC_CRT:BOOL=ON -DCMAKE_BUILD_TYPE:STRING=release
cmake --build .
```