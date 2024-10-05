#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 8000000UL

/* how many pixels are available to write? */
#define PIXELS 18

#define PIXEL_PORT PORTB
#define PIXEL_DDR  DDRB
#define PIXEL_BIT  PORTB2
/* PA5 is pin 8 */
/* softuart uses PA1 for RX, PA2 for TX (pins 12 and 11) */

#define SCALE(val) ((val) >> 2)

static uint8_t grb[PIXELS*3];


void set_pixel(uint8_t i, uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t offset = (i % PIXELS) * 3;
  grb[offset] = SCALE(g);
  grb[offset+1] = SCALE(r);
  grb[offset+2] = SCALE(b);
}

void write_pixels() 
{
  cli(); /* disable interrupts */
  asm volatile(
      "1:"                    "\n\t" /* outer loop: iterate bytes */
      "ld %[byte], %a[grb]+"  "\n\t"
      "ldi %[bits], 8"        "\n\t"
      "2:"                    "\n\t" /* inner loop: write a byte */
      "sbi %[port], %[pin]"   "\n\t" /* t = 0 */
      "sbrs %[byte], 7"       "\n\t" /* 2c if skip, 1c if no skip */
      "cbi %[port], %[pin]"   "\n\t" /* 2c, t1 = 375ns */
      "lsl %[byte]"           "\n\t" /* 1c, t = 500 (0) / 375 (1) */
      "dec %[bits]"           "\n\t" /* 1c, t = 625 (0) / 500 (1) */
      "nop"                   "\n\t" /* 1c, t = 750 (0) / 625 (1) */
      "cbi %[port], %[pin]"   "\n\t" /* 2c, t2= 875 (0) / 750 (1) */
      "brne 2b"               "\n\t" /* 2c if skip, 1c if not */
      "dec %[nbytes]"         "\n\t"
      "brne 1b"               "\n\t"
      :: [nbytes]  "d" (PIXELS * 3)  /* how many pixels to write */
      ,  [grb]     "w" (grb)         /* pointer to grb byte array */
      ,  [byte]    "d" (16)
      ,  [bits]    "d" (17)
      ,  [port]    "i" (_SFR_IO_ADDR(PIXEL_PORT))
      ,  [pin]     "i" (PIXEL_BIT)
      );
  _delay_us(10);
  sei(); /* enable interrupts */
}

int main(void)
{
	/* set pixel pin to output */
	PIXEL_DDR |= (1 << PIXEL_BIT);
	sei(); /* enable interrupts */
	//uint8_t pix = 0x03;
	while (1){
	
		for (uint8_t pix = 0x00; pix < 0x12; pix++) {
		set_pixel(pix, 0x00, 0x50, 0x00);
		write_pixels();
		_delay_ms(100);

		set_pixel(pix, 0x00, 0x00, 0x00);
		write_pixels();
		_delay_ms(5);
		}
		
	}
}
