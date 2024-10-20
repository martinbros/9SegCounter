#include "NineSegSao.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
/*
#define I2C_SLAVE_ADDRESS 0x4 // the 7-bit address (remember to change this when adapting this example)
// Get this from https://github.com/rambo/TinyWire
#include <TinyWireS.h>
// The default buffer size, Can't recall the scope of defines right now
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif
*/
#define redScale 0x40
#define greenScale 0x40
#define blueScale redScale * 2
uint8_t colors[][3] = {{0x00, 0x00, blueScale},
						{0x00, greenScale, 0x00},
						{0x00, greenScale, blueScale},
						{redScale, 0x00, 0x00},
						{redScale, 0x00, blueScale},
						{redScale, greenScale, 0x00},
						{redScale, greenScale, blueScale}};

volatile uint8_t display = 0x00;
volatile uint8_t update = 0x01;
/*
void receiveEvent(uint8_t howMany)
{
    if (howMany < 1)
    {
        // Sanity-check
        return;
    }
    if (howMany > TWI_RX_BUFFER_SIZE)
    {
        // Also insane number
        return;
    }

    display = TinyWireS.receive();
}
*/


ISR(PCINT0_vect){ // Interrupt for the clock pin
	
	if (!(PINA & (1<<PINA7))) // if PINA7 is low
	{
		display ++;
		display %= 100;
		update = 0x04;
	}
	
	//display ++;
	//update = 0x04;
}

int main(void)
{
	initializeIO();

	//TinyWireS.begin(I2C_SLAVE_ADDRESS);
    //TinyWireS.onReceive(receiveEvent);
    //TinyWireS.onRequest(requestEvent);

	sei(); /* enable interrupts */

	uint8_t button = 0x00;
	
	uint8_t oneColor = 0x00;
	uint8_t tenColor = 0x00;
	uint8_t address = tenColor * 10 + oneColor;
	
	display = eeprom_read_byte(&address);
	//uint8_t update = 0x01;
	
	while (1)
	{
		button = filter_keypad_original();

		switch (button)
		{
			case 0x01: // ones place increase
				display += 1;
				update = 0x01;
				break;

			case 0x02: // ones place decrease

				if (display != 0)
					display -= 1;
				else
					display = 0;
				update = 0x01;
				break;

			case 0x03: // ones place both button press
				oneColor += 1;
				oneColor %= 7;
				update = 0x02;
				break;

			case 0x04: // tens place increase
				display += 10;
				update = 0x01;
				break;

			case 0x08: // tens place decrease
				if (display > 10)
					display -= 10;
				else
					display = 0;
				update = 0x01;
				break;

			case 0x0c: // tens place both button press
				tenColor += 1;
				tenColor %= 7;
				update = 0x02;
				break;
		}

		if (display >= 100)
			display %= 100;

		address = tenColor * 10 + oneColor;

		if (update)  // update either number or address
		{
			if (update == 0x01) // Number is updated
				eeprom_write_byte(&address, display);
			if (update == 0x02) // Address is updated
				display = eeprom_read_byte(&address);

			set_digit(display % 10, 0, colors[oneColor % 7][0], colors[oneColor % 7][1], colors[oneColor % 7][2]);
			set_digit(display / 10 % 10, 1, colors[tenColor % 7][0], colors[tenColor % 7][1], colors[tenColor % 7][2]);
			write_pixels();
			write_pixels();
			update = 0x00;
		}

		
	}
}
