/* Sketch based on MCP4728 program address by Jan Knipper.
 * https://github.com/jknipper/mcp4728_program_address
 * 
 * Used with permission from the author.
 */

#include <Arduino.h>

#include "include/SoftI2cMaster.h"

// Change to LDAC pin here
#define LDAC_PIN 2

// Define address here (0-7)
#define NEW_ADDRESS 1

#ifdef CORE_TEENSY
  #define SDA_PIN 18
  #define SCL_PIN 19
#else // Arduino
  #define SDA_PIN A4
  #define SCL_PIN A5
#endif

SoftI2cMaster i2c;

void setup() {
  Serial.begin(9600);
  pinMode(LDAC_PIN, OUTPUT);

  i2c.init(SCL_PIN, SDA_PIN);
  delay(250);

  int addr_old = readAddress(LDAC_PIN);

  writeAddress(addr_old, NEW_ADDRESS, LDAC_PIN);
  delay(250);

  uint8_t addr = readAddress(LDAC_PIN);

  if (addr == NEW_ADDRESS) {
    Serial.println("Successfully set I2C address of the MCP4728! You may not remove the wire connecting the LDAC pin to the Arduino.");
    Serial.print("(New address is 0x6");
    Serial.print(addr);
    Serial.print(", old address was 0x6");
    Serial.print(addr_old);
    Serial.println(".)");
  } else {
    Serial.println("There was an error setting the new address. Please make sure all wirings are correct!");
  }
}

void loop() {}

int readAddress(int LDACpin) { // Read address bits in EEPROM
  digitalWrite(LDACpin, HIGH);
  int ack1 = i2c.start(0B00000000);
  int ack2 = i2c.ldacwrite(0B00001100, LDACpin); // modified command for LDAC pin latch
  int ack3 = i2c.restart(0B11000001);
  uint8_t address_bits = i2c.read(true);
  i2c.stop();
  delay(100);
  digitalWrite(LDACpin, HIGH);
  if (!(ack1 && ack2 && ack3)) {
    Serial.println("Error reading address bits!");
  }
  return (address_bits & 0B11100000) >> 5;
}

void writeAddress(int oldAddress, int newAddress, int LDACpin) {
  digitalWrite(LDACpin, HIGH);
  int ack1 = i2c.start( 0B11000000 | (oldAddress << 1));
  int ack2 = i2c.ldacwrite(0B01100001 | (oldAddress << 2), LDACpin); // modified command for LDAC pin latch
  int ack3 = i2c.write( 0B01100010 | (newAddress << 2));
  int ack4 = i2c.write( 0B01100011 | (newAddress << 2));
  i2c.stop();
  delay(100); // wait for eeprom write
  if(!(ack1 && ack2 && ack3 && ack4)) {
    Serial.println("Error writing address bits!");
  }
}
