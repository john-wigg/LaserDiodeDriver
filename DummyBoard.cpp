#include "DummyBoard.h"

int DummyBoard::Open() {
    is_open = true;
    return 0;
}

bool DummyBoard::DeviceIsOpen() const {
    return is_open;
}

int DummyBoard::WriteAnalogRelative(unsigned int channel, double relative_value)  { return 0; };
int DummyBoard::WriteDigital(unsigned int channel, bool value) { return 0; };