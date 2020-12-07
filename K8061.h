/* K8061.h
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
        int WriteAnalogRelative(unsigned int channel, double relative_value);
        int WriteDigital(unsigned int channel, bool value);
        bool DeviceIsOpen() const;
    private:
        std::string dev_;
        comedi_t *device_;
        int analog_n_channels_; /* Number of analog channels. */
        int digital_n_channels_; /* Number of digital channels. */
};

#endif // K8061_H_