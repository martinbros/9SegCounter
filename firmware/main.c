#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h> /* for puts_P macro support */
#include <util/delay.h>
#include "softuart.h"

/* how many pixels are available to write? */
#define PIXELS 16

#define PIXEL_PORT PORTA
#define PIXEL_DDR  DDRA
#define PIXEL_BIT  PORTA5
/* PA5 is pin 8 */
/* softuart uses PA1 for RX, PA2 for TX (pins 12 and 11) */

#define SCALE(val) ((val) >> 2)

static uint8_t grb[PIXELS*3];

/* rgb input buffer */
static uint8_t input[3];
/* how many pixels we've read */
static uint8_t pixels_read = 0;
/* the byte we're reading in */
static uint8_t byte = 0;
/* which nibble we're reading */
static uint8_t nibble = 0;

void set_pixel(uint8_t i, uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t offset = (i % PIXELS) * 3;
  grb[offset] = SCALE(g);
  grb[offset+1] = SCALE(r);
  grb[offset+2] = SCALE(b);
}

void write_pixels() {
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

void reset_input() {
  nibble = byte = pixels_read = 0;
  memset(&input, 0, 3);
}

void save_digit(char c)
{
  uint8_t value;
  if (c >= '0' && c <= '9') {
    value = c - '0';
  }
  else if (c >= 'a' && c <= 'f') {
    value = 0xa + (c - 'a');
  }
  else if (c >= 'A' && c <= 'F') {
    value = 0xa + (c - 'A');
  }
  else {
    return;
  }

  /* save the value in the current input and nibble */
  /* left to right, MSB first */
  if (nibble == 0) {
    value = value << 4;
  }
  input[byte] |= value;

  /* then advance to the next nibble, byte, or pixel */
  if (nibble == 0) {
    nibble++;
  }
  else {
    nibble = 0;
    byte += 1;
  }
  if (byte == 3) {
    set_pixel(pixels_read, input[0], input[1], input[2]);
    pixels_read += 1;
    byte = 0;
    memset(&input, 0, 3);
  }
  if (pixels_read == PIXELS) {
    reset_input();
    write_pixels();
    softuart_puts_P(".\r\n");
  }
}

int main(void)
{
  char c;

  /* set pixel pin to output */
  PIXEL_DDR |= (1 << PIXEL_BIT);
  softuart_init();
  sei(); /* enable interrupts */
  softuart_puts_P( "ready.\r\n" );
  write_pixels();

  for(;;) {
    if (softuart_kbhit()) {
      c = softuart_getchar();
      if (c == 'x') {
        reset_input();
        softuart_puts_P("x\r\n");
      }
      else if (c == 'q') {
        return 1;
      }
      else {
        save_digit(c);
      }
    }
  }

  return 0;
}
