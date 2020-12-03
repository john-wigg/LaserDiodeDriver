#include "Arduino.h"

#include <fstream>
#include <string>

#include "serial/serial.h"

Arduino::Arduino(std::string dev_path) {
    auto timeout = serial::Timeout::simpleTimeout(1000);
    dev_.setTimeout(timeout);
    dev_.setBaudrate(BAUD);
    dev_.setPort(dev_path);
}

Arduino::~Arduino() {
    dev_.write(std::vector<uint8_t>({CODE_CLOSE, '\n'}));
    dev_.close();
}

int Arduino::Open() {
    dev_.open();

    if (!dev_.isOpen()) return 1;

    dev_.write(std::vector<uint8_t>({CODE_OPEN, '\n'}));

    return 0;
}

bool Arduino::DeviceIsOpen() const {
    return dev_.isOpen();
}

int Arduino::WriteAnalogRelative(unsigned int channel, double relative_value) {
    if (!dev_.isOpen()) return 1;

    std::string value_string = std::to_string(relative_value);

    std::vector<uint8_t> sendbuf;

    sendbuf.push_back(CODE_WRITE_ANALOG);
    sendbuf.push_back(channel);
    std::copy(value_string.begin(), value_string.end(), std::back_inserter(sendbuf));
    sendbuf.push_back('\0'); // null terminator for atof on Arduino
    sendbuf.push_back('\n');

    dev_.write(sendbuf);

    return 0;
};

int Arduino::WriteDigital(unsigned int channel, bool value) {
    if (!dev_.isOpen()) return 1;

    std::vector<uint8_t> sendbuf;
    sendbuf.push_back(CODE_WRITE_DIGITAL);
    sendbuf.push_back(channel);

    if (value) sendbuf.push_back(0x01);
    else sendbuf.push_back(0x00);

    sendbuf.push_back('\n');

    dev_.write(sendbuf);

    return 0;
};