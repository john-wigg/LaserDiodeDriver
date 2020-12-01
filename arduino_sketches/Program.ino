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
#define MCP_BITS 12

// Buffer size for receiving data
#define BUFFER_SIZE 64

Adafruit_MCP4728 mcp1; // MCP4728 at address 0x60
Adafruit_MCP4728 mcp2; // MCP4728 at address 0x61

constexpr unsigned int mcp_max_value = 1 << MCP_BITS - 1;
constexpr char end_marker = '\n';

void setup() {
    Serial.begin(BAUD);
}

void loop () {
    char rc;
    static char buffer[];
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
    switch (code) {
        case CODE_OPEN: // Open the device
            mcp1.begin();
            mcp2.begin();
            break;
        case CODE_CLOSE: // Save current settings to EEPROM
            mcp1.saveToEEPROM();
            mcp2.savetoEEPROM();
            break;
        case CODE_WRITE_ANALOG: // Write to MCPs analog channel
            char ch = buffer[1];
            if (ch >= 8) return; // We only have 8 channels
            Adafruit_MCP4728 *dev;
            if (channel > 3) dev = &mcp2;
            else dev = &mcp1;
            char ch %= 4;

            double val = atof(buffer+3);
            dev->setChannelValue(ch, (uint16_t)(relative_value * mcp_max_value));
            break;
        case CODE_WRITE_DIGITAL: // Write do Arduino's digital channel
            char ch = buffer[1]; // channel
            if (ch >= 8) return; // We only use 8 channels.
            char val = buffer[2]; // value
            if (val) digitalWrite(ch, HIGH);
            else digitalWrite(ch, LOW);
            break;
    }
}