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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1 };

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
  delay(500);
}
#define TEMP  0
#define SV    1
#define KP    2
#define KI    3
#define KD    4
#define FACT  100.0

void loop() {
   uint16_t temp = ((uint16_t) thermocouple.readCelsius()*100);
   au16data[TEMP] = temp;
   slave.poll( au16data, 16 );
   
   float sv = au16data[SV]/FACT;
   float kp = au16data[KP]/FACT;
   float ki = au16data[KI]/FACT;
   float kd = au16data[KD]/FACT;
   
   lcdprint(temp,sv,kp,ki,kd);
   
   analogWrite(relay, 0);

   delay(500);
}

void lcdprint(uint16_t temp, float sv, float kp, float ki, float kd) {
  lcd.setCursor(0,0);
  lcd.print("t ");
  lcd.print(temp);
  lcd.print(" sv ");
  lcd.print(sv);
  lcd.setCursor(0,1);
  lcd.print("p");
  lcd.print(kp);
  lcd.print("i");
  lcd.print(ki);
  lcd.print("d");
  lcd.print(kd);
}
