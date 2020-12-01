#ifndef DUMMYBOARD_H_
#define DUMMYBOARD_H_

#include "InterfaceBoard.h"

class DummyBoard : public InterfaceBoard {
    public:
        ~DummyBoard(){};
        int Open();
        int WriteAnalogRelative(unsigned int channel, double relative_value);
        int WriteDigital(unsigned int channel, bool value);
        bool DeviceIsOpen() const;
    private:
        bool is_open;
};

#endif // DUMMYBOARD_H_