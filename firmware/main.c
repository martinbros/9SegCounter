#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/pgmspace.h> /* for puts_P macro support */
#include <util/delay.h>
#include "softuart.h"

#define PIXEL_PORT PORTA
#define PIXEL_DDR  DDRA
#define PIXEL_BIT  PORTA5
/* PA5 is pin 8 */
/* softuart uses PA1 for RX, PA2 for TX (pins 12 and 11) */

void check_serial()
{
  char c;
  if (softuart_kbhit()) {
    c = softuart_getchar();
    if (c & 0x1) {
      PIXEL_PORT |= _BV(PIXEL_BIT);
    }
    else {
      PIXEL_PORT &= ~_BV(PIXEL_BIT);
    }
    softuart_putchar(c);
    softuart_putchar('\r');
    softuart_putchar('\n');
  }
}

int main(void)
{
  /* set pixel pin to output */
  PIXEL_DDR |= (1 << PIXEL_BIT);
  softuart_init();
  sei(); /* enable interrupts */
  softuart_puts_P( "ready.\r\n" );

  for(;;) {
    check_serial();
  }

  return 0;
}
