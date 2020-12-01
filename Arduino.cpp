#include "Arduino.h"

#include <fstream>
#include <string>

#include "serial/serial.h"

Arduino::Arduino(std::string dev_path) {
    dev_path_ = dev_path;
}

Arduino::~Arduino() {
    dev_.write(std::vector<uint8_t>({CODE_CLOSE}));
    dev_.close();
}

int Arduino::Open() {
    dev_.setTimeout(serial::Timeout::simpleTimeout(1000));
    dev_.setBaudrate(BAUD);
    dev_.setPort(dev_path_);
    dev_.open();

    if (!dev_.isOpen()) return 1;

    return 0;
}

bool Arduino::DeviceIsOpen() const {
    return dev_.isOpen();
}

int Arduino::WriteAnalogRelative(unsigned int channel, double relative_value) {
    if (!dev_.isOpen()) return 1;

    dev_.write(std::vector<uint8_t>({CODE_WRITE_ANALOG, (uint8_t)channel}));
    dev_.write(std::to_string(relative_value) + '\0');

    return 0;
};

int Arduino::WriteDigital(unsigned int channel, bool value) {
    if (!dev_.isOpen()) return 1;
    
    dev_.write(std::vector<uint8_t>({CODE_WRITE_DIGITAL, (uint8_t)channel}));
    if (value) dev_.write(std::vector<uint8_t>({(0x01)}));
    else dev_.write(std::vector<uint8_t>({(0x00)}));
    return 0;
};