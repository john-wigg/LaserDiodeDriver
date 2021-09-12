/* Arduino.cpp
 *
 * Copyright (C) 2020 John Wigg
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

#include "Arduino.h"

#include <algorithm>
#include <fstream>
#include <string>

#include "serial/serial.h"

Arduino::Arduino(std::string dev_path) {
    auto timeout = serial::Timeout::simpleTimeout(1000);
    try {
        dev_.setTimeout(timeout);
        dev_.setBaudrate(BAUD);
        dev_.setPort(dev_path);
        dev_.setFlowcontrol(FLOWCTRL);
    } catch (...) {}
}

Arduino::~Arduino() {
    try {
        dev_.write(std::vector<uint8_t>({CODE_CLOSE, CODE_END_SEQUENCE}));
        dev_.close();
    } catch(...) {}
}

int Arduino::Open() {
    try {
        dev_.open(); // This guarantess is_open==true after so we only need to catch exceptions.
        dev_.write(std::vector<uint8_t>({CODE_OPEN, CODE_END_SEQUENCE}));
    } catch (...) {
        return 1;
    }

    return 0;
}

bool Arduino::DeviceIsOpen() const {
    try {
        return dev_.isOpen();
    } catch (...) {
        return false;
    }
}

int Arduino::WriteAnalogRelative(unsigned int channel, double relative_value) {
    std::string value_string = std::to_string(relative_value);

    std::vector<uint8_t> sendbuf;

    uint16_t value = (unsigned short)(relative_value * 65535);

    sendbuf.push_back(CODE_WRITE_ANALOG);
    sendbuf.push_back(channel);
    sendbuf.push_back((uint8_t)value);
    sendbuf.push_back((uint8_t)(value >> 8));
    sendbuf.push_back(CODE_END_SEQUENCE);

    try {
        dev_.write(sendbuf);
    } catch (...) {
        return 1;
    }

    return 0;
};

int Arduino::WriteDigital(unsigned int channel, bool value) {
    std::vector<uint8_t> sendbuf;
    sendbuf.push_back(CODE_WRITE_DIGITAL);
    sendbuf.push_back(channel);

    if (value) sendbuf.push_back(0x01);
    else sendbuf.push_back(0x00);

    sendbuf.push_back(CODE_END_SEQUENCE);

    try {
        dev_.write(sendbuf);
    } catch (...) {
        return 1;
    }

    return 0;
};
