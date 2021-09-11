/* Arduino.h
 *
 * Copyright (C) 2020, 2021 John Wigg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ARDUINO_H_
#define ARDUINO_H_

#include "InterfaceBoard.h"

#include <fstream>
#include <string>

#include "serial/serial.h"

#define BAUD 115200
#define FLOWCTRL "flowcontrol_software"

// Codes for communication via Serial
#define CODE_OPEN 0x00
#define CODE_CLOSE 0x01
#define CODE_WRITE_ANALOG 0x02
#define CODE_WRITE_DIGITAL 0x03
#define CODE_END_SEQUENCE 0x0A

class Arduino : public InterfaceBoard {
    public:
        Arduino(std::string dev_path);
        ~Arduino();
        int Open();
        int WriteAnalogRelative(unsigned int channel, double relative_value);
        int WriteDigital(unsigned int channel, bool value);
        bool DeviceIsOpen() const;
    private:
        serial::Serial dev_;
        bool is_open_ = false;
};

#endif // ARDUINO_H_
