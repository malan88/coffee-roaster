#include "Adafruit_MAX31855.h"
#include <ModbusRtu.h>
#include <LiquidCrystal.h>

#define MAXDO   2
#define MAXCS   3
#define MAXCLK  4

#define RS  13
#define EN  12
#define D4  11
#define D5  10
#define D6  9
#define D7  8

LiquidCrystal lcd(RS,EN,D4,D5,D6,D7);

uint16_t oldval = 0;
// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// data array for modbus network sharing
uint16_t au16data[16] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1 };

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  u8serno : serial port (use 0 for Serial)
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus slave(1,0,0); // this is slave @1 and RS-232 or USB-FTDI

int relay = 5;  
  
void setup() {
  slave.begin(19200); // 19200 baud, 8-bits, even, 1-bit stop
  // use Arduino pins 
  pinMode(relay, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("hello, world!");
  delay(500);
}

void loop() {
   //write current thermocouple value
   au16data[2] = ((uint16_t) thermocouple.readCelsius()*100);
   //poll modbus registers
   slave.poll( au16data, 16 );
   //write relay value using pwm
   float val = (au16data[4]/100.0);
   analogWrite(relay, val*255);
   lcd.setCursor(0, 1);
   lcd.print(val);
   delay(500);
}
