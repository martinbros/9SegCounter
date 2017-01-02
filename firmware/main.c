#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/pgmspace.h> /* for puts_P macro support */
#include <util/delay.h>
#include "softuart.h"

/* softuart uses PA1 for RX, PA2 for TX (pins 12 and 11) */

void check_serial()
{
  char c;
  if (softuart_kbhit()) {
    c = softuart_getchar();
    softuart_putchar(c);
    softuart_putchar('\r');
    softuart_putchar('\n');
  }
}

int main(void)
{
  softuart_init();
  sei(); /* enable interrupts */
  softuart_puts_P( "ready.\r\n" );

  while(1) {
    check_serial();
  }

  return 0;
}
