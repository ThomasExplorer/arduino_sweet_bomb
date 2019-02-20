#include <Wire.h> // for LCD LiquidCrystal_I2C library
#include <LiquidCrystal_I2C.h>
#include <SPI.h>  // for NRF24 radio library RF24
#include "RF24.h"

LiquidCrystal_I2C lcd(0x27,2, 1, 0, 4,5,6,7,3,POSITIVE);   // Set the LCD I2C address

#define LEDPIN 13

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins */
RF24 radio(9,10);
byte addresses[][6] = {"1Node","2Node"};

void setup()
{
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN, LOW);
  Serial.begin(9600);

  lcd.begin(16,2);               // initialize the lcd 
  lcd.home ();                   // go home
  lcd.print("sweet bomb");       

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.startListening();
}

void loop()
{
lcd.setCursor (0, 1 );        
lcd.print("message3");    
if (radio.available())
  {
  lcd.setCursor (0, 1 );        
  lcd.print("Radio Available");    
  }
}
