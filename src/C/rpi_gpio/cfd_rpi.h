#define BCM2708_PERI_BASE        0x3F000000 //Start of peripheral memory mappings
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) //Start of GPIO control regs

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

//I/O access pointer
volatile unsigned *gpio;


//GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

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

/* Lockout modes */
#define LOCKOUT_DISABLED	2
#define LOCKOUT_SHORT		1
#define LOCKOUT_LONG		0

/* Nowlin Modes */
#define NOWLIN_SHORT	0
#define NOWLIN_LONG		1

/* Test points */
#define TP_AVSS			0
#define TP_LOCKOUT		1
#define TP_LEADING_EDGE	#define TP_ZERO_CROSS	#define TP_ONESHOT		4

#define NEGATIVE_POL 1
#define POSITIVE_POL 0

#define ENABLE	0
#define DISABLE	1

void rpi_cleanup_gpio(void);
void rpi_configure(void);
void rpi_setup_io(void);
void delay_ns(long ns);
void set_addr_mode(char addr, char mode);
void set_data(char data);
void strobe_high(void);
void strobe_low(void);
void set_internal_agnd(char val);
void pulse_rst_l(void);
void set_polarity(char pol);
void set_gen(char val);
void set_write(void);
void set_read(void);
