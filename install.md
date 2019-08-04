# Building and installation

- You will need the following software packages to build acatl
  - cmake-3.15 or higher
- All other packages will be fetched by cmake

## Mac OS X 10.10 and newer
- Create a build directory in the source directory of acatl and change to it
- Call `cmake -DCMAKE_BUILD_TYPE=Release ..`
- Call `make`

## Linux
- clang++ 3.4 or higher compiler or g++ 4.8 or higher
- libc++ 3.4 or higher

- Create a build directory in the source directory of csvsqldb and change to it
- Call `cmake -DCMAKE_BUILD_TYPE=Release ..`
- Call `make`

