# blockregion
A minecraft style game built entirely from scratch


## Building

### Linux

Install dependencies using your native package manager.
```
sudo apt install \
    git \
    make \
    cmake \
    gcc-c++ \
    libgl-dev \
    libglu1-mesa-dev \
    libxinerama-dev \
    libxcursor-dev
```

Clone the repo and navigate into the base directory.
```
cd /path/to/blockregion/
```

Create build directory.
```
mkdir build
cd build
```

Create CMake project.
```
cmake ..
```

Build `blockregion`.
```
make -j blockregion
```


### Windows

