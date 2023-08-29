# Building the System Reference with libcamera
1. Open a terminal, source the project python virtual environment, and navigate to the `SystemReference` deployment
2. Set the PKG_CONFIG_PATH variable to the location where the libcamera pkgconfig directory is:
```bash
export PKG_CONFIG_PATH=<libcamera-build-directory>/lib/pkgconfig/
```
3. Next run `fprime-util generate <options>` and `fprime-util build <options>` to build the System Reference

If building natively, run:
```bash
fprime-util generate
fprime-util build
```

Or if building for an ARM 32-bit or ARM 64-bit Linux platform, run:

```bash
export ARM_TOOLS_PATH=<path to installation directory of ARM cross compilers>

#You can check to make sure the environment variable is set by running:
echo $ARM_TOOLS_PATH

# For Raspberry Pi/ARM 32-bit hardware
# In: Deployment Folder
fprime-util generate raspberrypi
fprime-util build raspberrypi

# For ARM 64-bit hardware
# In: Deployment Folder
fprime-util generate aarch64-linux
fprime-util build aarch64-linux
 ```