/* Program.ino
 *
 * Copyright (C) 2020-2022 John Wigg, Philipp Mueller and Daniel Schroeder, Jena University
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
#include <SPI.h>

#define BAUD 115200

// Codes for communication via Serial
#define CODE_OPEN 0x00
#define CODE_CLOSE 0x01
#define CODE_WRITE_ANALOG 0x02
#define CODE_WRITE_DIGITAL 0x03
#define CODE_SET_PWM 0x04
#define CODE_END_SEQUENCE 0x0A

// Adresses of MCPs
#define ADDR_MCP_1 0x60
#define ADDR_MCP_2 0x61

// MCP4728 is 12-bit
#define MAX_VALUE 4095

// Buffer size for receiving data
#define BUFFER_SIZE 64

// D0 and D1 are used for Serial comms, D2 is used to adress the second MCP4728 board, D3 is used
// as a pulse generator output for the fast laser switching. Block D4-D9 refers to Enable Laser 1 - 6.
// D11 Mosi, D12 Miso, D13 clock, chip select: 405nm = d10, 488nm = a6, 640nm = a7.
#define DIGITAL_PIN_OFFSET 4

Adafruit_MCP4728 mcp1; // MCP4728 at address 0x60
Adafruit_MCP4728 mcp2; // MCP4728 at address 0x61

constexpr char end_marker = CODE_END_SEQUENCE;

// Pulse-width modulation for MHz pulsing laser diodes
#define PWM_PIN (12UL)
#define PWM_PORT (1UL)
uint16_t pwm_seq[1] = {0};


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

    // hard-code the pulse-generator settings in for MHz pulsing lasers to 1MHz & 75% duty cycle
    set_pwm(4, 16);
}

void loop () {
    char rc;
    static char buffer[BUFFER_SIZE];
    static size_t pos;

    while (Serial.available() ) {
        rc = Serial.read();
        
        if (rc == end_marker) {
            parseBuffer(buffer, pos+1);
            pos = 0;
        } else if (pos < 62) {
            buffer[pos++] = rc;
        } else {
            pos = 0;
            break;
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
            if (ch >= 6) return; // We only have 6 channels
            Adafruit_MCP4728 *dev;
            if (ch > 2) dev = &mcp2; // Needs to be changed according to #channels per DAC
            else dev = &mcp1;
            
            ch %= 3; // Needs to be changed according to #channels per DAC

            uint8_t lower_bytes = buffer[2];
            uint8_t upper_bytes = buffer[3];
            uint16_t value = (upper_bytes << 8) | lower_bytes;
            float rel_val = (float)value / 65535;
            dev->setChannelValue((MCP4728_channel_t)ch, (uint16_t)(rel_val * MAX_VALUE), MCP4728_VREF_INTERNAL,
MCP4728_GAIN_1X);
        }
            break;
        case CODE_WRITE_DIGITAL: // Write to Arduino's digital channel
        {
            char ch = buffer[1]; // channel
            if (ch >= 6) return; // We only use 6 channels.
            char val = buffer[2]; // value
            if (val) digitalWrite(ch + DIGITAL_PIN_OFFSET, HIGH);
            else digitalWrite(ch + DIGITAL_PIN_OFFSET, LOW);
        }
            break;
        case CODE_SET_PWM: // Write to PWM channel
         {
             uint8_t duty = buffer[1];
             uint8_t top  = buffer[2];
             //uint16_t value = (upper_bytes << 8) | lower_bytes;
             set_pwm(duty, top);
         }
             break;
    }
}


void set_pwm(uint16_t duty, uint16_t top) // CLK = 16MHz
{
    pwm_seq[0] = (0 << 15) | duty; // Inverse polarity (bit 15), 1500us duty cycle
//      Configure PWM_PIN as output, and set it to 0
//  NRF_GPIO->DIRSET = (1 << PWM_PIN);
//  NRF_GPIO->OUTCLR = (1 << PWM_PIN);
  NRF_PWM0->PSEL.OUT[0] = (PWM_PIN << PWM_PSEL_OUT_PIN_Pos) | (PWM_PORT << PWM_PSEL_OUT_PORT_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
  NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
  NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
  NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
  NRF_PWM0->COUNTERTOP = (top << PWM_COUNTERTOP_COUNTERTOP_Pos); //1 msec
  NRF_PWM0->LOOP = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);
  NRF_PWM0->DECODER = (PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
  NRF_PWM0->SEQ[0].PTR = ((uint32_t)(pwm_seq) << PWM_SEQ_PTR_PTR_Pos);
  NRF_PWM0->SEQ[0].CNT = ((sizeof(pwm_seq) / sizeof(uint16_t)) << PWM_SEQ_CNT_CNT_Pos);
  NRF_PWM0->SEQ[0].REFRESH = 0;
  NRF_PWM0->SEQ[0].ENDDELAY = 0;
  NRF_PWM0->TASKS_SEQSTART[0] = 1;
}

// Explanation
// top: Frequency based on a 16MHz repetition rate. Counter counts up with a 16MHz clock.
// duty: Off duty time
