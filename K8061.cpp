/* K8061.cpp
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

#include "K8061.h"

#include <comedilib.h>

K8061::K8061(std::string dev) {
    dev_ = dev;
    bits_ = 0;
}

K8061::~K8061() {
    // Reset all channels.
    unsigned int bits = 0;
    unsigned int mask = (1 << digital_n_channels_) - 1;
    comedi_dio_bitfield2(device_, SUBDEV_DO, mask, &bits, 0);
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

int K8061::WriteAnalogRelative(unsigned int channel, double relative_value)
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

int K8061::WriteDigital(unsigned int channel, bool value) {
    if (channel > digital_n_channels_ - 1) { // invalid channel index
        return 1;
    }

    // Writing a bitfield prevents a bug where unwanted channels are turned on.
    unsigned int mask = (1 << digital_n_channels_) - 1;
    unsigned int bits;
    if (value) {
        bits = bits_ | 1 << channel;
    } else {
        bits = bits_ & ~(1 << channel);
    }
    int res = comedi_dio_bitfield2(device_, SUBDEV_DO, mask, &bits, 0);
    bits_ = bits; // Write actual state back.

    //int res = comedi_dio_write(device_, SUBDEV_DO, channel, (int)value);
    if (res != 1) { // error occured
        return 1;
    }
    return 0;
}

bool K8061::DeviceIsOpen() const {
    return device_ != NULL;
}
