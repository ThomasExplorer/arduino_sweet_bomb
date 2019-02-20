// SWEET BOMB USING ARDUINO UNO
// ============================

// Definitions and Ddeclarations for NeoPixels
// ==========================================
#include <FastLED.h>
#define NUM_LEDS 12
#define NEO_PXEL_PIN 5
CRGBArray<NUM_LEDS> leds;

// Definitions and Memory Declarations for Radio
// ==============================================
#include <SPI.h>  // for NRF24 radio library RF24
#include "RF24.h"
#define CE_PIN   9  // chip enable for radio
#define CSN_PIN 10  // chip select for radio
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
#define MESSAGE_LENGTH 8
#define CORRECT_CODE_LENGTH 6
char Message[MESSAGE_LENGTH] = {0,0,0,0,0,0,0,0};
char CorrectCode[MESSAGE_LENGTH] = {'8','8','9','6','9','2','-','-'};

// Definitions and Memory Declarations for Servo
// ==============================================
#include <Servo.h>
Servo myservo;

// Definitions and Memory Declarations for 4 Digit Display
// =======================================================
#include "TM1637.h"
#define CLK 2//pins definitions for TM1637 and can be changed to other ports
#define DIO 3
TM1637 tm1637(CLK,DIO);

#define LEDPIN  13

#define STARTING_TIME_IN_MINUTES  20
int StartTimeInSeconds = (STARTING_TIME_IN_MINUTES * 60);
int OldTimeNow = 0;
int TimeRemaining = StartTimeInSeconds; 

// Function to Check if Received Code Is Correct
// =============================================
bool IsCodeCorrect()
  {
    for (int i=0; i<CORRECT_CODE_LENGTH; i++)
      {
        if (Message[i] != CorrectCode[i])
          return false;
      }
    return true;
  }

// Initialise Devices
// ==================
void setup()
{
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN, LOW);
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL,NEO_PXEL_PIN>(leds, NUM_LEDS); 
  
  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  Serial.print("Initialised");  

  myservo.attach(6);
  myservo.write(90);

  tm1637.init();
  tm1637.set(BRIGHT_DARKEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7; 
}

//int MessageCount = 0;

// Main Program Loop
// =================
int StageNumber = 0;
//int attempt;
//int t;

void loop()
{
int TimeNow = millis()/1000; // calculate number of seconds elapsed
int ElapsedTime = TimeNow - OldTimeNow;
OldTimeNow = TimeNow;
TimeRemaining = TimeRemaining - ElapsedTime;
if (TimeRemaining < 0)
  TimeRemaining = 0;
int MinutesRemaining = TimeRemaining/60;
int SecondsRemaining = TimeRemaining%60;

static int FlashDelay = 80;

tm1637.display(0,MinutesRemaining/10);
tm1637.display(1,MinutesRemaining%10);
tm1637.display(2,SecondsRemaining/10);
tm1637.display(3,SecondsRemaining%10);

if (radio.available())
  {    
  radio.read( Message, MESSAGE_LENGTH);

  for (int i=0; i<MESSAGE_LENGTH; i++)
    {
    Serial.print(Message[i]);  
    Serial.print(" ");
    }
  Serial.print("\n");  

  byte MessageCode = Message[0];
  if (MessageCode == 'K')
    {
    StageNumber++;
    if (StageNumber == 1)
      {
      TimeRemaining = 60;
      FlashDelay = 40;
      }
    else if (StageNumber == 2)
      {
      TimeRemaining = 30;
      FlashDelay = 20;
      }
    else if (StageNumber == 3)
      {
      TimeRemaining = 15;
      FlashDelay = 8;
      }   
    }
  else if (MessageCode == 'R')
    {
    StageNumber = 0;
    TimeRemaining = (STARTING_TIME_IN_MINUTES * 60);
    FlashDelay = 80;
    }
  else if ((MessageCode >= '0') && (MessageCode <= '9'))
    {
    if (IsCodeCorrect())
      {
      myservo.write(180);
      delay(1000);
      myservo.write(90);
      TimeRemaining = 0;      
      }
    else
      {
      StageNumber = 0;
      TimeRemaining = (STARTING_TIME_IN_MINUTES * 60/10);
      FlashDelay = 80;        
      }
    }  
  }
  
for(int dot = 0; dot < NUM_LEDS; dot++) 
  { 
  if (TimeRemaining>0)
    leds[dot] = CRGB::Red;
  else 
    leds[dot] = CRGB::Black;
  FastLED.show();
  // clear this led for the next time around the loop
  leds[dot] = CRGB::Black;  
  delay(FlashDelay);
  }
}
