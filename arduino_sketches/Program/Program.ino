#include <Arduino.h>
#include <Adafruit_MCP4728.h>

#define BAUD 9600

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

// D0 and D1 are used for Serial comms
#define DIGITAL_PIN_OFFSET 2

Adafruit_MCP4728 mcp1; // MCP4728 at address 0x60
Adafruit_MCP4728 mcp2; // MCP4728 at address 0x61

constexpr char end_marker = '\n';

void setup() {
    Serial.begin(BAUD);
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
            mcp1.begin(0x60);
            mcp2.begin(0x61);
        }
            break;
        case CODE_CLOSE: // Save current settings to EEPROM
        {
            //mcp1.saveToEEPROM();
            //mcp2.saveToEEPROM();
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

            double rel_val = atof(buffer+2);
            dev->setChannelValue((MCP4728_channel_t)ch, (uint16_t)(rel_val * MAX_VALUE));
        }
            break;
        case CODE_WRITE_DIGITAL: // Write do Arduino's digital channel
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
