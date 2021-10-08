# LaserDiodeDriver

[![Linux Build](https://github.com/john-wigg/LaserDiodeDriver/workflows/linux%20build/badge.svg)](https://github.com/john-wigg/LaserDiodeDriver/actions)
[![Windows Build](https://github.com/john-wigg/LaserDiodeDriver/workflows/windows%20build/badge.svg)](https://github.com/john-wigg/LaserDiodeDriver/actions)

This [Micro-Manager](https://github.com/micro-manager/micro-manager) device adapter was developed by John Wigg and is part of the LaserEngine project by Daniel Schröder at the FSU Jena.

This adapter can be used to switch up to eight laser diodes as well as to linearly control their lasing output power.
Version 3 of the LaserEngine has an integrated [Arduino](https://www.arduino.cc/) together with two  [MCP4728](https://www.adafruit.com/product/4470) Quad-12bit DACs that act as an interface for this Adapter.

## EMU interface

An [EMU](https://micro-manager.org/wiki/EMU) plugin designed to be used with this adapter is available at https://github.com/john-wigg/LaserDiodeDriverUI.

## Table of contents
* [Known issues](#known-issues)
* [Prerequisites](#prerequisites)
* [Build instructions](#build-instructions-tested-on-windows-and-linux)
* [Arduino setup](#arduino-setup)
* [Outputs](#outputs)
* [Additional setup (Linux only)](#additional-setup-linux-only)
* [License](#license)

## Known issues

-

## Prerequisites
#### Micro-Manager

Please install [Micro-Manager 2.0.0 or above](https://micro-manager.org/Download_Micro-Manager_Latest_Release). This device-adapter is not compatible with version 1.4.

#### Install the Arduino IDE:

The Arduino IDE is only necessary to upload the Arduino program on the first time.

_Microsoft Windows_: 

[Arduino IDE](https://www.arduino.cc/en/software)

_Debian & Derivates_:
```
apt-get install arduino
```

_Archlinux_:
```
pacman -Sy arduino
```

## Install Micro-Manager device adapter

### Pre-Built Binaries

You can find pre-built binaries on the github repository [here](https://github.com/john-wigg/LaserDiodeDriver/releases).

After installing the binaries, please proceed at "Arduino setup".

### Build instructions (tested on Windows and Linux)

**Alternatively**, you can build your device adapter from the sources.

[CMake](https://cmake.org/) is used as the build system for this device adapter. Below is a breakdown of the build process. It is assumed that Micro-Manager is already installed on your Linux or Windows machine.

1. Clone this respository *with updated submodules* to a directory:
```
git clone --recurse-submodules --remote-submodules https://github.com/john-wigg/LaserDiodeDriver
```

This will download the device adapter source as well as the *latest* `mmCoreAndDevices` source which is needed for compilation.

2. Open the LaserDiodeDriver source directory you just cloned:

```
cd LaserDiodeDriver
```

3. Create a build directory and enter it:

```
mkdir build
cd build
```

4. Generate the build files and specify the source directory of the Micro-Manager core:

```
cmake ..
```
(If you did not clone the repository recursively and want to supply your own version of `mmCoreAndDevices`, you can specify the path to the directory using `cmake .. -DMMROOT=path/to/mmCoreAndDevices`.)

5. Run the build:

```
cmake --build .
```

6. Rename the generated shared library:

```
mv libmmgr_dal_LaserDiodeDriver.so libmmgr_dal_LaserDiodeDriver.so.0
```

**Note**: Skip this step for windows builds.

7. Copy the shared library to the installation directory of Micro-Manager/ImageJ (may require root permissions):

```
cp libmmgr_dal_LaserDiodeDriver.so.0 /path/to/ImageJ
```

**Note**: On Windows, the expected file extension is `.dll` instead of `.so.0`.

8. Run Micro-Manager and create a new hardware configuration with the `LaserDiodeDriver` device adapter.

## Arduino setup

1. For initialising the setup, connect the `LDAC` pin of *one* MCP4728 to the `D2` pin. To do so, the LaserEngine v3 has a jumper switch included on the breadboard, that just needs to be set on.

1. Install the [Adafruit_MCP4728](https://github.com/adafruit/Adafruit_MCP4728) library. This can be done directly from the Arduino IDE under `Tools > Manage Libraries...`. Make sure you install the library by *Adafruit*.

![](media/library_manager.png)

3. Download the [arduino_sketches/Setup](arduino_sketches/Setup) directory and upload the `Setup.ino` sketch to your Arduino using the [Arduino IDE](https://www.arduino.cc/en/software). This will set the address of the MCP4728 which has its `LDAC` pin connected to the Arduino (pink wire) to `0x61` so it can be controlled individually.

4. If the setup sketch has executed successfully, you may remove the jumper switch (v3) or pink wire (v2) connecting the `LDAC` pin to the Arduino.
 
5. Download the [arduino_sketches/Program](arduino_sketches/Program) directory and upload the `Program.ino` sketch to your Arduino.

6. The Arduino is now successfully programmed to communicate with this Micro-Manager device adapter.

7. In Micro-Manager, open Devices -> Hardware Configuration Wizard and at LaserDiodeDriver, choose "Arduino" as Device Type.

## Additional setup (Linux only)

If you want to use the LaserEngine without the need for `sudo`, add your user to the uucp group:
```
usermod -a -G uucp user
```

## EMU plugin

After successfully installing this device adapter, it is highly recommended to install the [EMU plugin]( https://github.com/john-wigg/LaserDiodeDriverUI) for an easy-to-use graphical interface.

Please contact us for feedback or open an issue on github if you experience some difficulties.

# Alternative Interfaces for this device adapter

Apart from the above preferred interface method, two more options are available.

### Arduino on a breadboard

The following instruction will walk you through the process of building the setup on a breadboard and configuring the Arduino. The examples assume that you are using an [Arduino Nano 33 BLE](https://store.arduino.cc/products/arduino-nano-33-ble), however, other boards with 3.3V digital outputs should work as well. 5V output boards are not supported.

1. Connect the `SDA` and `SCL` pins of the two MCP4728s to the corresponding pins of the Arduino (`A4` and `A5`, respectively on the Arduino Nano). Connect the `VCC` pins to the `3V` pin of the Arduino and tie all `GND` pins together. 

 2. The complete Fritzing diagram is shown below:

![](media/fritzings/arduino_breadboard.png)

3. Proceed with the above "Arduino setup" to program the DACs adresses and to install the program.

#### Digital Outputs

In Micro-Manager, the digital outputs are numbered `1` to `8`.
On the Arduino Nano 33 BLE, these will be D3 to D10, in ascending order.

#### Analog Outputs

In Micro-Manager, the analog and digital outputs are numbered `1` to `8`, which corresponds to the outputs on the LaserEngine Diodes.

On the MCP4728s, these will be the analog outputs A to D, whereas the MCP4728 with address `0x60` (the one closer to the Arduino in the figure above) will correspond to the outputs `1` to `4` and the other MCP4728 will correspond to the outputs `5` to `8`.

### Velleman k8061

Moreover, the adapter can also communicate with the [Velleman k8061](https://www.velleman.eu/products/view/?lang=en&id=364910) interface board by using the [comedi](https://www.comedi.org/) library to communicate with the Velleman K8061. Due to the availability of the comedi library, this method is only supported on Linux OSes.

1. The [comedi](https://www.comedi.org/) library is required to run the Velleman K8061 board on Linux. It can be installed with

`sudo apt-get install libcomedi-dev`

2. If you want to use the adapter without the need for sudo, add a .rules file with the following content to /etc/udev/rules.d/:

```
KERNEL=="comedi0", MODE="0666"
```

3. After installing the device adapter (see section "Install Micro-Manager device adapter"), open Devices -> Hardware Configuration Wizard and at LaserDiodeDriver, choose "K8061" as Device Type.

4. Save the configuration.

5. You should now be all set to use the k8061 analog and digital outputs 1-8 as an interface to control your laser drivers.

# License

This project is licensed under the [MIT license](LICENSE).

Exempt from this is the `Setup.ino` sketch which uses the [SoftI2cMaster](https://github.com/TrippyLighting/SoftI2cMaster) library and is licensed under the [GNU GPLv3](arduino_sketches/Setup/LICENSE).

The `Setup.ino` sketch has been originally created by Jan Knipper (https://github.com/jknipper/mcp4728_program_address) and is used [with the author's permission](https://github.com/jknipper/mcp4728_program_address/issues/1).

This project uses the [serial](https://github.com/wjwwood/serial) library which is available under the MIT license.
