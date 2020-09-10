#ifndef _INTERFACEBOARD_H_
#define _INTERFACEBOARD_H_

#include <string>

class InterfaceBoard
{
    public:
        virtual ~InterfaceBoard(){};
        virtual int Open() = 0;
        virtual int WriteAnalogRelative(unsigned int channel, double relative_value) const = 0;
        virtual int WriteDigital(unsigned int channel, bool value) const = 0;
        virtual bool DeviceIsOpen() const = 0;
};

#endif // _INTERFACEBOARD_H_