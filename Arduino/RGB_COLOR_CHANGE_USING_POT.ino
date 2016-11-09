//Program: RGB Color Change with Potentiometer
//Programmer: Brett Reinhard
//Date: 11/1/2016
//Description: Changes color of LED's based on User Input from Potentiometer.

int const Red = 3,  /* Pin 3 */ Blue = 5, /* Pin 5 */ Green = 6; /* Pin 6 */
int potPin = 2;    // Input Pin for Potentiometer
int potVal = 0;       // Value Read From Potentiometer, then remapped to fit the color spectrum
void RGB(int R, int G, int B) {
analogWrite(Red,R);                           //Writes passed value to Red Pin
analogWrite(Green,G);                         //Writes passed value to Green Pin
analogWrite(Blue,B);}                         //Writes passed value to Blue Pin

void setup() {}                               //No Setup Necessary
void loop() {
  potVal = analogRead(potPin);                //Reads potVal from the Potentiometer
  potVal = map(potVal, 0, 1023, 0 ,1535);     //Remaps potVal to fit color spectrum

  
  /*----------------Checked potVal to see where on the spectrum it is set to.----------------*/
  
  if (potVal >= 0 && potVal < 256){
/*1:*/    RGB(255,potVal,0);                  //RGB(255,0->255,0) RED to YELLOW
  }else if(potVal >= 256 && potVal < 512){
/*2:*/    RGB(-potVal+511,255,0);             //RGB(255->0,255,0) YELLOW to GREEN
  }else if(potVal >=512 && potVal < 768){
/*3:*/    RGB(0,255,potVal-512);              //RGB(0,255,0->255) GREEN to TEAL
  }else if(potVal >= 768 && potVal < 1024){
/*4:*/    RGB(0,-potVal+1023,255);            //RGB(0,255->,255) TEAL to BLUE
  }else if(potVal >=1024 && potVal < 1280){
/*5:*/    RGB(potVal-1024,0,255);             //RGB(0->255,0,255) BLUE to MAGENTA
  }else if(potVal >= 1280 && potVal <1536){
/*6:*/    RGB(255,0,-potVal+1535);            //RGB(255,0,255->0) MAGENTA to RED
  }
}

 



