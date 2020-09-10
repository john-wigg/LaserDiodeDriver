# LaserDiodeDriver

This [Micro-Manager](https://github.com/micro-manager/micro-manager) device adapter is part of a project by Daniel Schröder at the FSU Jena. It can be used to control laser diodes via an interface board. The only currently supported board is the Velleman K8061. The adapter uses the [comedi](https://www.comedi.org/) library to communicate with the K8061 and is thus only available on Linux.

## EMU interface

An [EMU](https://micro-manager.org/wiki/EMU) plugin deigned to be used with this adapter is available at https://github.com/john-wigg/LaserDiodeDriverUI.

## Known bugs

Sometimes, when setting the number of laser diodes, the hardware configuration manager crashes (see [this issue](https://github.com/john-wigg/LaserDiodeDriver/issues/1)). A workaround is to leave the number of diodes at 1 on initial configuration and change the number manually in the generated config file. Note that in order to access the hardware configuration wizard again, an empty configuration may have to be loaded.

## Build instructions

[CMake](https://cmake.org/) is used the build system for this device adapter. Below is a breakdown of the build process. It is assumed that Micro-Manager is already installed as an ImageJ plugin.

1. Clone the Micro-Manager 2 source:
```
git clone https://github.com/micro-manager/micro-manager
```

2. Clone this respository to a directory:
```
git clone https://github.com/john-wigg/LaserDiodeDriver
```

3. Open the LaserDiodeDriver source directory you just cloned:
```
cd LaserDiodeDriver
```

4. Create a build directory and enter it:
```
mkdir build
cd build
```

5. Generate the build files and specify the source directory of Micro-Manager:
```
cmake .. -DMMROOT=path/to/micro-manager
```
(If the `MMROOT` option is not specified, it is assumed that the `LaserDiodeDriver` directory is located inside the `DeviceAdapters` or `TestDeviceAdapters` directory inside Micro-Manager's source tree.)

**Note**: If the compilation completes but you get "undefined reference" errors when trying to open the adapter in Micro-Manager, the path to Micro-Manager was probably incorrect.

6. Run the build:
```
cmake --build .
```

7. Rename the generated shared library:
```
mv libmmgr_dal_LaserDiodeDriver.so libmmgr_dal_LaserDiodeDriver.so.0
```

8. Copy the shared library to the installation directory of Micro-Manager/ImageJ (may require root permissions):
```
cp libmmgr_dal_LaserDiver.so.0 /path/to/ImageJ
```

9. Run Micro-Manager and create a new hardware configuration with the `LaserDiodeDriver` device adapter.

## Additional setup

If you want to run Micro-Manager with the adapter enabled without `sudo`, add a `.rules` file with the following content to `/etc/udev/rules.d/`:

```
KERNEL=="comedi0", MODE="0666"
```
