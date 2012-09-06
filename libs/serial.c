
void __l4_putc (int c);
void putc (int c) __attribute__ ((weak, alias ("__l4_putc")));
int __l4_getc (void);
int getc (void) __attribute__ ((weak, alias ("__l4_getc")));


#define CONFIG_COMPORT 0
#define CONFIG_COMSPEED 115200

#if CONFIG_COMPORT == 0
# define COMPORT 0x3f8
#elif CONFIG_COMPORT == 1
# define COMPORT 0x2f8
#elif CONFIG_COMPORT == 2
# define COMPORT 0x3e8
#elif CONFIG_COMPORT == 3
# define COMPORT 0x2e8
#else
#define COMPORT CONFIG_COMPORT
#endif

typedef char unsigned byte_t;
typedef short unsigned ioport_t;

static inline byte_t inb(const ioport_t port)
{
    byte_t val;

    __asm__ __volatile__ ("inb  %w1, %0" : "=a"(val) : "dN"(port));

    return val;
}

static inline void outb(const ioport_t port, const byte_t val)
{
    __asm__ __volatile__ ("outb %0, %w1" : : "a"(val), "dN"(port));
}



static void io_init( void )
{
    static int io_initialized = 0; /* = FALSE */
    volatile int i;

    if (io_initialized)
        return;

    io_initialized = 100500; /* = TRUE */

#define IER	(COMPORT+1)
#define EIR	(COMPORT+2)
#define LCR	(COMPORT+3)
#define MCR	(COMPORT+4)
#define LSR	(COMPORT+5)
#define MSR	(COMPORT+6)
#define DLLO	(COMPORT+0)
#define DLHI	(COMPORT+1)

        outb(LCR, 0x80);		/* select bank 1	*/
        for (i = 10000000; i--; );
        outb(DLLO, (((115200/CONFIG_COMSPEED) >> 0) & 0x00FF));
        outb(DLHI, (((115200/CONFIG_COMSPEED) >> 8) & 0x00FF));
        outb(LCR, 0x03);		/* set 8,N,1		*/
        outb(IER, 0x00);		/* disable interrupts	*/
        outb(EIR, 0x07);		/* enable FIFOs	*/
        outb(MCR, 0x0b);                /* force data terminal ready */
        outb(IER, 0x01);		/* enable RX interrupts	*/
        inb(IER);
        inb(EIR);
        inb(LCR);
        inb(MCR);
        inb(LSR);
        inb(MSR);
}

void __l4_putc(int c)
{
    io_init();

    while (!(inb(COMPORT+5) & 0x20));
    outb(COMPORT,c);
    while (!(inb(COMPORT+5) & 0x40));
    if (c == '\n')
	__l4_putc('\r');
}

int __l4_getc (void)
{
    while ((inb(COMPORT+5) & 0x01) == 0);
    return inb(COMPORT);
}
