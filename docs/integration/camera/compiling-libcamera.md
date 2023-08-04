## Compiling libcamera for Native Linux
To compile libcamera for Native Linux, do the below:
```bash
meson build -Dprefix=<path-to-system-reference-root>/libcamera/build/
cd build
ninja
ninja install
```

## Cross Compiling libcamera for ARM Linux

Before building the System Reference for ARM Linux, libcamera will first need to be cross-compiled for ARM 64-bit or ARM 32-bit Linux.

1. In a terminal, set the RPI_TOOLS variable to the path to the ARM cross compilers and add it to your PATH:
```bash
export RPI_TOOLS=<path to installation directory of ARM cross compiler>
export PATH=$RPI_TOOLS:$PATH
```

3. Navigate to the libcamera directory and cross-compile for ARM Linux by doing:

For ARM 64-bit:
```bash
meson build -Dprefix=<path-to-system-reference-root>/libcamera/build/ --cross-file ../libcamera-aarch64.txt
cd build
ninja
ninja install
```

For ARM 32-bit:
```bash
meson build -Dprefix=<path-to-system-reference-root>/libcamera/build/ --cross-file ../libcamera-aarch32.txt
cd build
ninja
ninja install
```