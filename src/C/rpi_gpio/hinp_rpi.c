#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hinp_rpi.h"
#include <sched.h>

//Pin accessors -- Use the Broadcom GPIO number *not* the phyiscal pin header number
//const unsigned char DATA[8] = {5,6,13,19,26,16,20,21}; //data bus -- used to load
const unsigned char DATA[8] = {7,8,25,24,23,18,15,14}; //data bus -- used to load configuration registers
const unsigned char MODE[4] = {7,8,25,24}; //address bus -- just an alias of the upper nibble of the data bus
const unsigned char ADDR[4] = {23,18,15,14}; //mode bus -- just an alias of the lower nibble of the data bus
//const unsigned char NEG_POL = 7; //negative polarity enable -- POS,NEG = 0,1
const unsigned char WRITE = 12; //bidirectional bus control -- IN,OUT = 1,0
const unsigned char STB = 2; //data/address strobe -- addr/mode latched on rising edge, data latched on falling edge
const unsigned char GEN = 20; //global enable -- OUTPUT,NO_OUTPUT = 1,0
const unsigned char EXT_AGND = 26; //external AGND enable -- INTERNAL,EXTERNAL = 0,1
const unsigned char RST_L = 16; //low active reset -- ensures one-shot works properly.
const unsigned char ADC_CONV = 3; //ADC convert signal
const unsigned char LOW_GAIN_SDO = 27; //Low gain ADC data 
const unsigned char TVC_SDO = 4; //TVC ADC data 
const unsigned char HIGH_GAIN_SDO = 9; //High gain ADC data
const unsigned char ADC_SCK = 11; //ADC data clock for all three ADCs
const unsigned char ACQ_ALL = 21; //Acquire all
const unsigned char FORCE_RESET = 13; //Force digital reset and advance priority encoder during readout.
const unsigned char COMMON_STOP = 19; //Stop all TVCs.
const unsigned char TAKE_EVENT = 6; //Start readout.
const unsigned char OR_OUT = 17; //Logical OR of all channel hit registers.


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
    
    INP_GPIO(RST_L);
    OUT_GPIO(RST_L);
	GPIO_SET = 1 << RST_L;   
    
    INP_GPIO(ADC_CONV);
    OUT_GPIO(ADC_CONV);
    GPIO_CLR = 1 << ADC_CONV;
    
    INP_GPIO(ADC_SCK);
    OUT_GPIO(ADC_SCK);
    GPIO_SET = 1 << ADC_SCK; 
    
    INP_GPIO(LOW_GAIN_SDO);
    
    INP_GPIO(HIGH_GAIN_SDO);
    
    INP_GPIO(TVC_SDO);
    
    INP_GPIO(ACQ_ALL);
    OUT_GPIO(ACQ_ALL);
    GPIO_CLR = 1 << ACQ_ALL;
    
    INP_GPIO(FORCE_RESET);
    OUT_GPIO(FORCE_RESET);
    GPIO_CLR = 1 << ACQ_ALL;
    
    INP_GPIO(COMMON_STOP);
    OUT_GPIO(COMMON_STOP);
    GPIO_CLR = 1 << COMMON_STOP;
    
    INP_GPIO(TAKE_EVENT);
    OUT_GPIO(TAKE_EVENT);
    GPIO_CLR = 1 << TAKE_EVENT;
    
    INP_GPIO(OR_OUT);      
 
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


void set_conv(char state)
{
    
	if(state)
	{
		GPIO_SET = 1 << ADC_CONV;
	}
	else
	{
		GPIO_CLR = 1 << ADC_CONV;
	}
}

void set_acq_all(char state)
{
	if(state)
	{
		GPIO_SET = 1 << ACQ_ALL;
	}
	else
	{
		GPIO_CLR = 1 << ACQ_ALL;
	}
}

void set_common_stop(char state)
{
	if(state)
	{
		GPIO_SET = 1 << COMMON_STOP;
	}
	else
	{
		GPIO_CLR = 1 << COMMON_STOP;
	}
}

void force_reset_low(void)
{
    GPIO_CLR = 1 << FORCE_RESET;
}

void force_reset_high(void)
{
    GPIO_SET = 1 << FORCE_RESET;
}

void set_take_event(char state)
{
	if(state)
	{
		GPIO_SET = 1 << TAKE_EVENT;
	}
	else
	{
		GPIO_CLR = 1 << TAKE_EVENT;
	}
}


struct adc_readings read_adcs(void)
{
    struct adc_readings readings = {0};
    set_conv(1);
    delay_ns(CONV_TIME_NS);
    set_conv(0);
    delay_ns(CONV_SETUP_NS);
    for(iter = 0; iter < 16; iter++)
    {
        GPIO_CLR = 1 << ADC_SCK;
        delay_ns(SCK_PERIOD_NS/2);
        GPIO_SET = 1 << ADC_SCK;
        //delay(ns(SCK_PERIOD_NS/2);
        readings.tvc |= (GET_GPIO(TVC_SDO)) ? (1 << iter):0;
        readings.low_gain |= (GET_GPIO(LOW_GAIN_SDO)) ? (1 << iter):0;
        readings.high_gain |= (GET_GPIO(HIGH_GAIN_SDO)) ? (1 << iter):0;
    }
    
    return readings;
}

void init_adcs(void)
{
    set_conv(1);
    delay_ns(CONV_TIME_NS);
    set_conv(0);
    delay_ns(CONV_SETUP_NS);
    for(iter = 0; iter < 16; iter++)
    {
        GPIO_CLR = 1 << ADC_SCK;
        delay_ns(SCK_PERIOD_NS/2);
        GPIO_SET = 1 << ADC_SCK;
        delay_ns(SCK_PERIOD_NS/2);
    }
}

char read_channel_number(void)
{
    char channel = 0;
    for(iter = 0; iter < ADDR_BITS; iter++)
    {
        channel |= GET_GPIO(ADDR[iter]) ? (1 << iter):0;
    }
    
    return channel;
}

char read_or_out_pin(void)
{
    return GET_GPIO(OR_OUT) ? 1:0;
}
