#ifndef ARDUINO_H_
#define ARDUINO_H_

#include "InterfaceBoard.h"

#include <fstream>
#include <string>

#include "serial/serial.h"

#define BAUD 9600

// Codes for communication via Serial
#define CODE_OPEN 0x00
#define CODE_CLOSE 0x01
#define CODE_WRITE_ANALOG 0x02
#define CODE_WRITE_DIGITAL 0x03

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