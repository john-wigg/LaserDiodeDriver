#include "K8061.h"

#include <comedilib.h>

K8061::K8061(std::string dev) {
    std::string dev_ = dev;
}

int K8061::Open() {
    
	device_ = comedi_open(dev_.c_str());
	if (device_ == NULL) {
		comedi_perror(dev_.c_str());
		return 1; // error occured
	}
    analog_n_channels_ = comedi_get_n_channels(device_, SUBDEV_AO);
    digital_n_channels_ = comedi_get_n_channels(device_, SUBDEV_DO);

    return 0;
}

int K8061::WriteAnalogRelative(unsigned int channel, double relative_value) const
{
    if (channel > analog_n_channels_ - 1) { // invalid channel index
        return 1;
    }

    lsampl_t maxdata = comedi_get_maxdata(device_, SUBDEV_AO, channel);

    int res = comedi_data_write(device_, SUBDEV_AO, channel, 0, AREF_GROUND, (lsampl_t)(relative_value * maxdata));
    if (res != 1) { // error occured
        return 1;
    }
        
    return 0;
}

int K8061::WriteDigital(unsigned int channel, bool value) const {
    if (channel > digital_n_channels_ - 1) { // invalid channel index
        return 1;
    }
    int res = comedi_dio_write(device_, SUBDEV_DO, channel, (int)value);
    if (res != 1) { // error occured
        return 1;
    }
    return 0;
}

bool K8061::DeviceIsOpen() const {
    return device_ != NULL;
}