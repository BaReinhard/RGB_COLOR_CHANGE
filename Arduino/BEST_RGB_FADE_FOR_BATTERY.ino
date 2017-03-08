//Program: RGB Color Change
//Programmer: Brett Reinhard
//Date: 10/20/2016
//Description: Changes color of LED's within the Pumpkin, with smooth transitions.
// Imports LowPower lib for power saving sleep
#include LowPower.h
int const Red = 3,  /* Pin 3 */ Blue = 5, /* Pin 5 */ Green = 6; /* Pin 6 */
void RGB(int R, int G, int B) {
analogWrite(Red,R); analogWrite(Green,G); analogWrite(Blue,B); delay(25);}
void setup() {}
void loop() {
int r=255,g=0,b=0;for (int i = 0; i < 256; ++i){RGB(r,i,b);}
r = 255; g = 255;b = 0; for (int i = 255; i >= 0; --i){RGB(i,g,b); } 
r = 0;g = 255;b = 0; for (int i = 0; i < 256; ++i){RGB(r,g,i);}
r = 0;g = 255;b = 255; for (int i = 255; i >= 0; --i){RGB(r,i,b);}
r = 0;g = 0;b = 255; for (int i = 0; i < 256; ++i){RGB(i,g,b);}
r = 255;g = 0;b = 255; for (int i = 255; i >= 0; --i){RGB(r,g,i);}
// Saves battery or power by shutting off multiple components
LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
     }
 



