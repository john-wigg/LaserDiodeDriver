/* Program.ino
 *
 * Copyright (C) 2020,2021 John Wigg
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

#include <Arduino.h>
#include <Adafruit_MCP4728.h>

#define BAUD 115200

// Codes for communication via Serial
#define CODE_OPEN 0x00
#define CODE_CLOSE 0x01
#define CODE_WRITE_ANALOG 0x02
#define CODE_WRITE_DIGITAL 0x03

// Adresses of MCPs
#define ADDR_MCP_1 0x60
#define ADDR_MCP_2 0x61

// MCP4728 is 12-bit
#define MAX_VALUE 4095

// Buffer size for receiving data
#define BUFFER_SIZE 64

// D0 and D1 are used for Serial comms, D2 is used to adress the second MCP4728 board.
#define DIGITAL_PIN_OFFSET 3

Adafruit_MCP4728 mcp1; // MCP4728 at address 0x60
Adafruit_MCP4728 mcp2; // MCP4728 at address 0x61

constexpr char end_marker = '\n';

void setup() {
    Serial.begin(BAUD);

    // Set pinMode to OUTPUT and set to LOW.
    for (int ch = 0; ch < 8; ++ch) {
        pinMode(DIGITAL_PIN_OFFSET+ch, OUTPUT);
        digitalWrite(DIGITAL_PIN_OFFSET+ch, LOW);
    }
    
    mcp1.begin(0x60);
    mcp2.begin(0x61);

    for (int ch = 0; ch < 4; ++ch) {
      mcp1.setChannelValue((MCP4728_channel_t)ch, (uint16_t)0);
      mcp2.setChannelValue((MCP4728_channel_t)ch, (uint16_t)0);
    }    
}

void loop () {
    char rc;
    static char buffer[BUFFER_SIZE];
    static size_t pos;

    while (Serial.available() > 0) {
        rc = Serial.read();
        if (rc == end_marker) {
            parseBuffer(buffer, pos+1);
            pos = 0;
        } else {
            buffer[pos++] = rc;
        }
    }
}

void parseBuffer(char *buffer, size_t length) {
    if (length == 0) return; // Buffer is empty, can't parse
    char code = buffer[0];
    switch (code)  {
        case CODE_OPEN: // Open the device
        {
        }
            break;
        case CODE_CLOSE:
        {
            // Turn lasers off.
            for (int ch = 0; ch < 8; ++ch) {
                digitalWrite(DIGITAL_PIN_OFFSET+ch, LOW);
            }

            for (int ch = 0; ch < 4; ++ch) {
                mcp1.setChannelValue((MCP4728_channel_t)ch, (uint16_t)0);
                mcp2.setChannelValue((MCP4728_channel_t)ch, (uint16_t)0);
            }
                
            mcp1.saveToEEPROM();
            mcp2.saveToEEPROM();
        }
            break;
        case CODE_WRITE_ANALOG: // Write to MCPs analog channel
        {
            char ch = buffer[1];
            if (ch >= 8) return; // We only have 8 channels
            Adafruit_MCP4728 *dev;
            if (ch > 3) dev = &mcp2;
            else dev = &mcp1;
            
            ch %= 4;

            uint8_t lower_bytes = buffer[2];
            uint8_t upper_bytes = buffer[3];
            uint16_t value = (upper_bytes << 8) | lower_bytes;
            float rel_val = (float)value / 65535;
            dev->setChannelValue((MCP4728_channel_t)ch, (uint16_t)(rel_val * MAX_VALUE));
        }
            break;
        case CODE_WRITE_DIGITAL: // Write to Arduino's digital channel
        {
            char ch = buffer[1]; // channel
            if (ch >= 8) return; // We only use 8 channels.
            char val = buffer[2]; // value
            if (val) digitalWrite(ch + DIGITAL_PIN_OFFSET, HIGH);
            else digitalWrite(ch + DIGITAL_PIN_OFFSET, LOW);
        }
            break;
    }
}
