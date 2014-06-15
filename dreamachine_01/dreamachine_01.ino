#include "LPD8806.h"
#include "SPI.h"

// Simple test for 160 (5 meters) of LPD8806-based RGB LED strip

/*****************************************************************************/

// Number of RGB LEDs in strand:
int nLEDs = 160;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

// You can optionally use hardware SPI for faster writes, just leave out
// the data and clock pin parameters.  But this does limit use to very
// specific pins on the Arduino.  For "classic" Arduinos (Uno, Duemilanove,
// etc.), data = pin 11, clock = pin 13.  For Arduino Mega, data = pin 51,
// clock = pin 52.  For 32u4 Breakout Board+ and Teensy, data = pin B2,
// clock = pin B1.  For Leonardo, this can ONLY be done on the ICSP pins.
//LPD8806 strip = LPD8806(nLEDs);

uint32_t q[159];

/*
color array = {waveform, initial value, inc rate, max val}
*/
#define WFORM 0
#define WVAL 1
#define RATE 2
#define MAXVAL 3

#define TRI 0
#define SAWUP 1
#define SAWDN 2
#define RAND 3

float r[] = {TRI,0,.3,64};
float g[] = {TRI,0,.2,16};
float b[] = {TRI,0,.5,32};

void stepwave(float*);

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  int i;
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();

  for(i=0; i<strip.numPixels(); i++){
    q[i] = strip.Color(0,0,0);
  }
  pushQ();
}

void loop() {
  stepwave(r);
  stepwave(g);
  stepwave(b); 

  q[0] = strip.Color(int(r[WVAL]),int(g[WVAL]),int(b[WVAL]));
  pushQ();
  shiftQ();
}

void stepwave(float* ca){

  long randNumber;

  switch(int(ca[WFORM])){
    case TRI:
      ca[WVAL] += ca[RATE];
      if(ca[WVAL] > ca[MAXVAL]){
        ca[WVAL] = ca[MAXVAL];
        ca[RATE] = 0 - ca[RATE];
      } else if(ca[WVAL] < 0){
        ca[WVAL] = 0;
        ca[RATE] = 0 - ca[RATE];
      }
      break;
    case SAWUP:
      ca[WVAL] += ca[RATE];
      if(ca[WVAL] > ca[MAXVAL]){
        ca[WVAL] = 0;
      }
      break;
    case SAWDN:
      ca[WVAL] -= ca[RATE];
      if(ca[WVAL] < 0){
        ca[WVAL] = ca[MAXVAL];
      }
      break;
    case RAND:
      randNumber = random(10);
      if (randNumber > 4){
        ca[WVAL] += ca[RATE];
      } else {
        ca[WVAL] -= ca[RATE];
      }
      if(ca[WVAL] < 0){
        ca[WVAL] = 0;
      } 
      else if(ca[WVAL] > ca[MAXVAL]){   
        ca[WVAL] = ca[MAXVAL];
      }
  }
}

void pushQ(){
  int i;
  for(i=0; i<strip.numPixels(); i++){
    strip.setPixelColor(i,q[i]);
  }
  strip.show(); 
}  

void shiftQ(){
  memmove(&q[1], &q[0], sizeof(q) - sizeof(*q));
}
