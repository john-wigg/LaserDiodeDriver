# LaserDiodeDriver

[![Linux Build](https://github.com/john-wigg/LaserDiodeDriver/workflows/linux%20build/badge.svg)](https://github.com/john-wigg/LaserDiodeDriver/actions)
[![Windows Build](https://github.com/john-wigg/LaserDiodeDriver/workflows/windows%20build/badge.svg)](https://github.com/john-wigg/LaserDiodeDriver/actions)

This [Micro-Manager](https://github.com/micro-manager/micro-manager) device adapter is part of a project by Daniel Schröder at the FSU Jena and can be used to control laser diodes. Both the [Velleman K8061](https://www.velleman.eu/products/view/?lang=en&id=364910) as well as a setup using an [Arduino](https://www.arduino.cc/) together with two [MCP4728](https://www.adafruit.com/product/4470) DACs can be used with the adapter.

The adapter uses the [comedi](https://www.comedi.org/) library to communicate with the Velleman K8061. Thus, on Windows the Velleman K8061 is not supported. However, the adapter can still be used with the Arduino setup.

## EMU interface

An [EMU](https://micro-manager.org/wiki/EMU) plugin designed to be used with this adapter is available at https://github.com/john-wigg/LaserDiodeDriverUI.

## Known issues

Sometimes, when setting the number of laser diodes, the hardware configuration manager crashes (see [this issue](https://github.com/john-wigg/LaserDiodeDriver/issues/1)). A workaround is to leave the number of diodes at 1 on initial configuration and change the number manually in the generated config file. Note that in order to access the hardware configuration wizard again, an empty configuration may have to be loaded.

## Prerequisites

### Linux

The [comedi](https://www.comedi.org/) library is required to run the Velleman K8061 board on Linux. It can be installed with

`sudo apt-get install libcomedi-dev`


## Build instructions (Tested on Windows and Linux)

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

**Note**: If the compilation completes but you get "undefined reference" errors when trying to open the adapter in Micro-Manager, the path to Micro-Manager was probably incorrect. Note that if not absolute, paths are relative to the repos root directory (the directory containing `CMakeListst.txt`).

6. Run the build:
```
cmake --build .
```

7. Rename the generated shared library:
```
mv libmmgr_dal_LaserDiodeDriver.so libmmgr_dal_LaserDiodeDriver.so.0
```

TODO: File ending is `.dll` on Windows. Has it still to be renamed?

8. Copy the shared library to the installation directory of Micro-Manager/ImageJ (may require root permissions):
```
cp libmmgr_dal_LaserDiodeDriver.so.0 /path/to/ImageJ
```

TODO: File ending is `.dll` on Windows.

9. Run Micro-Manager and create a new hardware configuration with the `LaserDiodeDriver` device adapter.

## Arduino setup

If you want to use an Arduino connected to two [MCP4728](https://learn.adafruit.com/adafruit-mcp4728-i2c-quad-dac) DACs you need to setup your Arduino first. You only have to follow these steps once for the initial setup.

1. Connect the two MACP4728s to the Arduino as shown in the Fritzing diagram below (here shown for an Arduino Nano):
![](media/fritzings/arduino_breadboard.png)
2. Install the [Adafruit_MCP4728](https://github.com/adafruit/Adafruit_MCP4728) library. This can be done directly from the Arduino IDE under `Tools > Manage Libraries...`. Make sure you install the library by **Adafruit**.
![](media/library_manager.png)
1. Using the [Arduino IDE](https://www.arduino.cc/en/software), upload the `Setup.ino` [sketch](arduino_sketches/Setup.ino) to your Arduino. This will set the address of the MCP4728 which has its LDAC pin connected to the D2 pin of the Arduino (pink wire) to `0x61` so it can be controlled individually.
1. If the setup sketch has executed successfully, you may remove the pink wire connecting the LDAC pin to the Arduino.
1. Upload the `Program.ino` [sketch](arduino_sketches/Program.ino) to your Arduino.

## Additional setup (Linux only)

If you want to run Micro-Manager with the adapter enabled with the K8061 board without `sudo`, add a `.rules` file with the following content to `/etc/udev/rules.d/`:

```
KERNEL=="comedi0", MODE="0666"
```

# License

This project is licensed under the MIT license.

Excluded from this is the Arduino Setup Sketch which uses the [SoftI2cMaster](https://github.com/TrippyLighting/SoftI2cMaster) library and is licensed under the GNU GPLv3.

The `Setup.ino` sketch has been originally created by Jan Knipper (https://github.com/jknipper/mcp4728_program_address) and is used [with the author's permission](https://github.com/jknipper/mcp4728_program_address/issues/1).