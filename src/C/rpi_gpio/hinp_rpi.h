#ifndef HINP_RPI_H
#define HINP_RPI_H

//#define RPI4

#ifdef RPI4
    #define BCM2708_PERI_BASE        0xFE000000 //Start of peripheral memory mappings
#else
    #define BCM2708_PERI_BASE        0x3F000000 //Start of peripheral memory mappings
#endif

#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) //Start of GPIO control regs

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

//GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

#define CONV_TIME_NS    3200
#define CONV_SETUP_NS   60
#define SCK_PERIOD_NS   50

/* Number of channles */
#define CHANNELS 16

/* Register modes */
#define NOWLIN_MODE		0b00000000
#define AGND_MODE		0b00000001
#define ONESHOT_MODE	0b00000001
#define LOCKOUT_MODE	0b00000101
#define DAC_MODE		0b00000110
#define GMODE			0b00001000

/* Channel Addresses */
#define CH0		0b00000000
#define CH1		0b00010000
#define CH2		0b00100000
#define CH3		0b00110000
#define CH4		0b01000000
#define CH5		0b01010000
#define CH6		0b01100000
#define CH7		0b01110000
#define CH8		0b10000000
#define CH9		0b10010000
#define CH10	0b10100000
#define CH11	0b10110000
#define CH12	0b11000000
#define CH13	0b11010000
#define	CH14	0b11100000
#define CH15	0b11110000
#define COMMON	0b00000000

/* Bit widths */
#define DATA_BITS	8
#define MODE_BITS	4
#define ADDR_BITS	4

/* GUI settings below this line */

/* AGND settings */
#define AGND_1_36		0
#define AGND_1_43		1
#define AGND_1_49		2
#define AGND_1_56		3
#define	AGND_1_63		4
#define	AGND_1_69		5
#define AGND_1_77		6
#define AGND_1_84		7
#define AGND_NOMINAL	4
#define AGND_MAX		7
#define AGND_MIN		0

/* Digital auto reset modes */
#define AR_DIGITAL_100      0
#define AR_DIGITAL_1000		1

/* ADC Buffer Modes */
#define BUFF_NEG_50     0
#define BUFF_NEG_25     1
#define BUFF_25         3
#define BUFF_50         4

/* TVC Modes */
#define TVC_2US     1
#define TVC_500NS   0

/* Nowlin Modes */
#define NOWLIN_SHORT	0
#define NOWLIN_LONG		1

#define COLLECT_HOLES       1
#define COLLECT_ELECTRONS   0

#define ENABLE	0
#define DISABLE	1

#define EVEN_PULSER 1
#define ODD_PULSER  2
#define NO_PULSER   0

struct adc_readings
{
    short tvc;
    short low_gain;
    short high_gain;
};

void rpi_cleanup_gpio(void);
void rpi_configure(void);
void rpi_setup_io(void);
void delay_ns(long ns);
void set_addr_mode(char addr, char mode);
void set_data(char data);
void strobe_high(void);
void strobe_low(void);
void set_internal_agnd(char val);
void pulse_rst_l(int val);
void set_gen(char val);
void set_write(void);
void set_read(void);
void set_conv(char state);
void set_acq_all(char state);
void set_common_stop(char state);
void force_reset_low(void);
void force_reset_high(void);
void set_take_event(char state);
struct adc_readings read_adcs(void);
void init_adcs(void);
char read_channel_number(void);
char read_addr_dat(void);
char read_or_out_pin(void);
void toggle_dummy(void);

#endif
