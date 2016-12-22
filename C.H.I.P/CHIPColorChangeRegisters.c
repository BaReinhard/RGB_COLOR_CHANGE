// Used for Raspberry Pi #define BCM2708_PERI_BASE        0x3F000000 // Base Address For BCM on Raspberry Pi 2 & 3, if using other RPI set to 0x20000000 
#define GPIO_BASE                0x01C20800 // GPIO controller, can be found at 0x200000 away from BASE
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
void setup_io();

#define INP_GPIO(g,reg_addr) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
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

void main(){
	setup_io();

}

















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
 
 
}
