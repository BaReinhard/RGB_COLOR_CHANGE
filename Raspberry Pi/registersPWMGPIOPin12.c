
//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program
//  15-January-2012
//  Dom and Gert
//  Revised: 15-Feb-2013
 
 
// Access from ARM Running Linux
 
#define BCM2708_PERI_BASE        0x3F000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
 
 
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
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
#define NANO_SECOND_MULTIPLIER  1000000  // 1 millisecond = 1,000,000 Nanoseconds

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
 
#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
 
#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock
 
void setup_io();
void BPWM(int dutyCyc);
 
void printButton(int g)
{
  if (GET_GPIO(g)) // !=0 <-> bit is 1 <- port is HIGH=3.3V
    printf("Button pressed!\n");
  else // port is LOW=0V
    printf("Button released!\n");
}
 
int main(int argc, char **argv)
{
  int g,rep;
 
  // Set up gpi pointer for direct register access
  setup_io();
 

 
  
    INP_GPIO(12); 
    OUT_GPIO(12);
  
 
  for (rep=0; rep<=100; rep++)
  {
     BPWM(rep);
  }
  for (rep=100;rep>=0;rep--){
    BPWM(rep);
  }
 
  return 0;
 
} // main
 

 void BPWM(int dutyCyc)
 {
  int i;
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
  GPIO_SET = 1 << 12;
  nanosleep(on.tv_nsec,NULL);
  GPIO_CLR = 1 << 12;
  nanosleep(off.tv_nsec,NULL);
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
