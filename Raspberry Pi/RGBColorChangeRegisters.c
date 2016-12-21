//  Access Registers, Basics taken from elinux.org
//  Authors: Brett Reinhard 
#define BCM2708_PERI_BASE        0x3F000000 //Base Address For BCM on Raspberry Pi 2 & 3, if using other RPI set to 0x20000000 
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller, can be found at 0x200000 away from BASE */
 
 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)
 
int  mem_fd;
void *gpio_map;
 
// I/O access
volatile unsigned *gpio;
 
 
// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
// Using INP before OUT, will allow for the proper bits to be set to 0, before setting as an output
// If using the pin strictly for INP, you do not need to call OUT_GPIO
// Usage for these macros are as follows: INP_GPIO(12); Sets Pin 12 as On for Input
// INP_GPIO(10);OUT_GPIO(10); Sets pin 10 as Output, making sure to use INP before OUT to ensure proper bits are set
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
// Allows for setting a pin for an alternate function such as for communicating with SPI or I2C
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
#define NANO_SECOND_MULTIPLIER  1000000  // 1 millisecond = 1,000,000 Nanoseconds This is used for nanosleep

// Sets THE GPIO SET Register with the Proper information, Proper usage is as follows:
// GPIO_SET = 1 << 18; Turns Pin 18 to HIGH,ON.
// GPIO_CLR = 1 << 18; Turns Pin 18 to LOW,OFF.
// Note: Setting GPIO_SET as GPIO_SET = 0 << 18; May cause problems when using multiple pins.
// Use the SET macro to turn on, and CLR macro to turn off.
#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

//
#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH aka 2 ^ pinNum, if pin 3 HIGH = 2 ^ 3 = 8
 
#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock
 
void setup_io();
void SoftPWM(int dutyCyc, int Pin);
 
void printButton(int g)
{
  if (GET_GPIO(g)) // !=0 <-> bit is 1 <- port is HIGH=3.3V
    printf("Button pressed!\n");
  else // port is LOW=0V
    printf("Button released!\n");
}
 
int main(int argc, char **argv)
{
  int RED,GREEN,BLUE,i;
  RED = 33;
  GREEN = 35;
  BLUE = 37;
  int g,rep;
 
  // Set up gpio pointer for direct register access
  setup_io();
 

 
    // Sets RED Pin as Output
    INP_GPIO(RED); 
    OUT_GPIO(RED);
    // Sets GREEN Pin as Output
    INP_GPIO(GREEN); 
    OUT_GPIO(GREEN);
    //Sets BLUE Pin as Output
    INP_GPIO(BLUE); 
    OUT_GPIO(BLUE);
//  while(1){
    //I Have left in Previously Used Arduino C Code as a reference to see the difference between the two:
    //  void RGB(int R, int G, int B) {analogWrite(Red,R); analogWrite(Green,G); analogWrite(Blue,B); delay(25);}
//r = 255;g = 0;b = 0; for (int i = 0; i < 256; ++i){RGB(r,i,b);}
    GPIO_SET = 1 << RED;
    GPIO_CLR = 1 << BLUE;
    GPIO_CLR = 1 << GREEN;
    for (i = 0; i < 101; i++){SoftPWM(i,GREEN);}
//r = 255; g = 255;b = 0; for (int i = 255; i >= 0; --i){RGB(i,g,b); } 
    //GPIO_SET = 1 << RED;
    //GPIO_SET = 1 << GREEN;
    //GPIO_CLR = 1 << BLUE;
    for(i = 100; i>0;i--){SoftPWM(i,RED);}
//r = 0;g = 255;b = 0; for (int i = 0; i < 256; ++i){RGB(r,g,i);}
    //GPIO_CLR = 1 << RED;
    //GPIO_SET = 1 << GREEN;
    //GPIO_CLR = 1 << BLUE;
    for (i = 0; i < 101; i++){SoftPWM(i,BLUE);}
//r = 0;g = 255;b = 255; for (int i = 255; i >= 0; --i){RGB(r,i,b);}
    //GPIO_CLR = 1 << RED;
    //GPIO_SET = 1 << GREEN;
    //GPIO_SET = 1 << BLUE;
    for (i = 100; i>0;i--){SoftPWM(i,GREEN);}
//r = 0;g = 0;b = 255; for (int i = 0; i < 256; ++i){RGB(i,g,b);}
    //GPIO_CLR = 1 << RED;
    //GPIO_CLR = 1 << GREEN;
    //GPIO_SET = 1 << BLUE;
    for (i = 0; i < 101 ; i++){SoftPWM(i,RED);}
//r = 255;g = 0;b = 255; for (int i = 255; i >= 0; --i){RGB(r,g,i);}
    //GPIO_SET = 1 << RED;
    //GPIO_CLR = 1 << GREEN;
    //GPIO_SET = 1 << BLUE; 
    for(i = 100; i >0; i--){SoftPWM(i,BLUE);}
// }
  
 
  return 0;
 
} // main
 

 void SoftPWM(int dutyCyc, int Pin)
 {
  int i, readVal;
  unsigned int diff =0;
  struct timespec on,off,timer,start,stop;
  clock_gettime(CLOCK_MONOTONIC_RAW ,&start);

  on.tv_sec = 0;
  on.tv_nsec = (dutyCyc/100) * NANO_SECOND_MULTIPLIER;
  off.tv_sec = 0;
  off.tv_nsec = ((100 - dutyCyc)/100) * NANO_SECOND_MULTIPLIER;
  timer.tv_sec = 0;
  timer.tv_nsec = 25 * NANO_SECOND_MULTIPLIER;
   
 
 while(diff < 25){
  if (dutyCyc == 0){
    GPIO_CLR = 1 << Pin;
    readVal = GET_GPIO(Pin);
    printf("%x\n",readVal);
    nanosleep(&timer,NULL);
  }
  else if(dutyCyc == 100){
    GPIO_SET = 1 << Pin;
    readVal = GET_GPIO(Pin);
    printf("%x\n",readVal);
    nanosleep(&timer,NULL);
  }
  else{
  GPIO_SET = 1 << Pin;
  readVal = GET_GPIO(Pin);
  printf("%x\n",readVal);
  nanosleep(&on,NULL);
  GPIO_CLR = 1 << Pin;
  readVal = GET_GPIO(Pin);
  printf("%x\n",readVal);
  nanosleep(&off,NULL);
}
  clock_gettime(CLOCK_MONOTONIC_RAW,&stop);
  diff = (stop.tv_nsec - start.tv_nsec)/ NANO_SECOND_MULTIPLIER;
 }
 
 }
 
//
// Set up a memory regions to access GPIO
//
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }
 
   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );
 
   close(mem_fd); //No need to keep mem_fd open after mmap
 
   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }
 
   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;
 
 
} // setup_io
