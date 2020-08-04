#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "cfd_rpi.h"
#include <sched.h>

//Pin accessors
//const unsigned char DATA[8] = {5,6,13,19,26,16,20,21}; //data bus -- used to load
const unsigned char DATA[8] = {21,20,16,26,19,13,6,5}; //data bus -- used to load configuration registers
const unsigned char ADDR[4] = {5,6,13,19}; //address bus -- just an alias of the upper nibble of the data bus
const unsigned char MODE[4] = {26,16,20,21}; //mode bus -- just an alias of the lower nibble of the data bus
const unsigned char NEG_POL = 7; //negative polarity enable -- POS,NEG = 0,1
const unsigned char WRITE = 24; //bidirectional bus control -- IN,OUT = 1,0
const unsigned char STB = 25; //data/address strobe -- addr/mode latched on rising edge, data latched on falling edge
const unsigned char GEN = 9; //global enable -- OUTPUT,NO_OUTPUT = 1,0
const unsigned char EXT_AGND = 23; //external AGND enable -- INTERNAL,EXTERNAL = 0,1
const unsigned char RST_L = 12; //low active reset -- ensures one-shot works properly.

int iter; //iterator for loops

void set_write()
{
	GPIO_SET = 1 << WRITE;
}

void set_read()
{
	GPIO_CLR = 1 << WRITE;
}

void set_addr_mode(char addr, char mode)
{
	
	for(iter = 0; iter < MODE_BITS; iter++)
	{
		if(addr & (1 << iter))
		{
			GPIO_SET = 1 << ADDR[iter];
		}
		else
		{
			GPIO_CLR = 1 << ADDR[iter];
		}

		if(mode & (1 << iter))
		{
			GPIO_SET = 1 << MODE[iter];
		}
		else
		{
			GPIO_CLR = 1 << MODE[iter];
		}	
	}
	
}

void set_data(char data)
{
	
	for(iter = 0; iter < DATA_BITS; iter++)
	{
		if(data & (1 << iter))
		{
			GPIO_SET = 1 << DATA[iter];
		}
		else
		{
			GPIO_CLR = 1 << DATA[iter];
		}
	}
}

void strobe_high()
{
	GPIO_SET = 1 << STB;
}

void strobe_low()
{
	GPIO_CLR = 1 << STB;
}

void set_internal_agnd(char val)
{
	if(val == ENABLE)
	{
		GPIO_CLR = 1 << EXT_AGND;
	}
	else
	{
		GPIO_SET = 1 << EXT_AGND;
	}
}

void pulse_rst_l()
{
	GPIO_CLR = 1 << RST_L;
	delay_ns(5000);
	GPIO_SET = 1 << RST_L;
}

void set_polarity(char pol)
{
	if(pol == NEGATIVE_POL)
	{
		GPIO_SET = 1 << NEG_POL;
	}
	else
	{
		GPIO_CLR = 1 << NEG_POL;
	}
}

void set_gen(char val)
{
	if(val)
	{
		GPIO_SET = 1 << GEN;
	}
	else
	{
		GPIO_CLR = 1 << GEN;
	}
}

void delay_ns(long ns)
{
	struct timespec t, rt;
	
	clock_gettime(CLOCK_REALTIME, &rt);
	rt.tv_nsec = 0;
	clock_settime(CLOCK_REALTIME, &rt);
	clock_gettime(CLOCK_REALTIME, &rt);
	rt.tv_nsec += ns;

	while(1)
	{
		clock_gettime(CLOCK_REALTIME, &t);
		if(rt.tv_nsec > t.tv_nsec)
			break;
	}
}

void rpi_setup_io()
{
   // open /dev/gpiomem 
   if ((mem_fd = open("/dev/gpiomem", O_RDWR|O_SYNC) ) < 0) 
   {
      printf("can't open /dev/gpiomem \n");
      exit(-1);
   }

   //mmap GPIO 
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) 
   {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

   //Point access pointer at mapped region.
   gpio = (volatile unsigned *)gpio_map;
}

//setup GPIO modes and set initial control line states.
void rpi_configure()
{
    //Must use INP_GPIO before OUT_GPIO. INP_GPIO will ensure the two MSBs for FSEL are 0
    //OUT_GPIO will set LSB of FSEL to 1 so that the function select is 001 which is output
    INP_GPIO(WRITE);
    OUT_GPIO(WRITE);
	GPIO_CLR = 1 << WRITE;
	INP_GPIO(STB);
    OUT_GPIO(STB);
	GPIO_CLR = 1 << STB;
	delay_ns(1000);
	GPIO_SET = 1 << STB;
    INP_GPIO(GEN);
    OUT_GPIO(GEN);
	GPIO_CLR = 1 << GEN;
	delay_ns(1000);
	GPIO_SET = 1 << GEN;
    INP_GPIO(EXT_AGND);
    OUT_GPIO(EXT_AGND);
	GPIO_CLR = 1 << EXT_AGND;
	delay_ns(1000);
	GPIO_SET = 1 << EXT_AGND;
    INP_GPIO(NEG_POL);
    OUT_GPIO(NEG_POL);
	GPIO_CLR = 1 << NEG_POL;
	delay_ns(1000);
	GPIO_SET = 1 << NEG_POL;
    INP_GPIO(RST_L);
    OUT_GPIO(RST_L);
	GPIO_SET = 1 << RST_L;   
 
    for(iter = 0; iter < 8; iter++)
    {
        INP_GPIO(DATA[iter]);
        OUT_GPIO(DATA[iter]);
		GPIO_CLR = 1 << DATA[iter];
    }

    GPIO_SET = 1 << WRITE; //write a 1 to the GPIOSET1 register bit 5
    GPIO_CLR = 1 << RST_L; //bring RST low to force oneshot state low
	usleep(1000); //hold RST low for 1 ms
    GPIO_SET = 1 << RST_L; //bring RST high to enable oneshot
}

void rpi_cleanup_gpio()
{
	for(iter = 0; iter < 32; iter++)
	{
		GPIO_CLR = 1 << iter;
		INP_GPIO(iter);
	}
}

