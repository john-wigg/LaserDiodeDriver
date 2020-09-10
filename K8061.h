#ifndef K8061_H_
#define K8061_H_

#include "InterfaceBoard.h"

#include <comedilib.h>

#include <string>

#define SUBDEV_AO 1 /* analog output */
#define SUBDEV_DO 3 /* digital output */

class K8061 : public InterfaceBoard {
    public:
        K8061(std::string dev);
        ~K8061(){};
        int Open();
        int WriteAnalogRelative(unsigned int channel, double relative_value) const;
        int WriteDigital(unsigned int channel, bool value) const;
        bool DeviceIsOpen() const;
    private:
        std::string dev_;
        comedi_t *device_;
        int analog_n_channels_; /* Number of analog channels. */
        int digital_n_channels_; /* Number of digital channels. */
};

#endif // K8061_H_