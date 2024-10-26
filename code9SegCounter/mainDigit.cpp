#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

extern "C"
{
	#include "NineSegSao.h"
}

#define I2C_SLAVE_ADDRESS 0x4 // the 7-bit address (remember to change this when adapting this example)
// Get this from https://github.com/rambo/TinyWire
#include "TinyWireS.h"
// The default buffer size, Can't recall the scope of defines right now
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

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
volatile uint8_t update = 0x02;
uint8_t *address;

volatile uint8_t i2c_regs[] =
{
    0x01, 
    0x02, 
    0x03, 
    0x04,
    0x05,
    0x06 
};
const uint8_t reg_size=sizeof(i2c_regs);
volatile uint8_t reg_position;
volatile uint8_t sentSize = 0x00;

void disableLedPin()
{
	DDRB = 0x00;
	PORTB &= ~(0x04);
}

void enableLedPin()
{
	DDRB |= 0x04;
}

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

    reg_position = 0x00;
    sentSize = howMany;
    while(howMany--)
    {
        i2c_regs[reg_position] = TinyWireS.receive();
        reg_position++;
        if (reg_position >= reg_size)
        {
            reg_position = 0;
        }
    }

    update = 0x03;
}

void requestEvent()
{  
    TinyWireS.send(eeprom_read_byte(address));
}

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

	TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
    TinyWireS.onRequest(requestEvent);

	sei(); /* enable interrupts */

	uint8_t button = 0x00;
	
	uint16_t oneColorIdx = 0x00;
	uint16_t tenColorIdx = 0x00;
	
	uint8_t oneRedColor = colors[oneColorIdx][0];
	uint8_t oneGrnColor = colors[oneColorIdx][1];
	uint8_t oneBluColor = colors[oneColorIdx][2];
	uint8_t tenRedColor = colors[oneColorIdx][0];
	uint8_t tenGrnColor = colors[oneColorIdx][1];
	uint8_t tenBluColor = colors[oneColorIdx][2];

	//uint8_t *address;
	address = (uint8_t *) (tenColorIdx * 10 + oneColorIdx);


	while (1)
	{
		button = filter_keypad_original();  // Get button press status

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
				oneColorIdx += 1;
				oneColorIdx %= 7;

				oneRedColor = colors[oneColorIdx][0];
				oneGrnColor = colors[oneColorIdx][1];
				oneBluColor = colors[oneColorIdx][2];

				address = (uint8_t *) (tenColorIdx * 10 + oneColorIdx);
				
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
				tenColorIdx += 1;
				tenColorIdx %= 7;

				tenRedColor = colors[tenColorIdx][0];
				tenGrnColor = colors[tenColorIdx][1];
				tenBluColor = colors[tenColorIdx][2];

				address = (uint8_t *) (tenColorIdx * 10 + oneColorIdx);

				update = 0x02;
				break;
		}

		switch(sentSize)
		{
			case 0x01:  // One byte via I2C

				display = i2c_regs[0];  // Update Display
				sentSize = 0x00;
				break;
			
			case 0x02:  // Two bytes via I2C
				address = (uint8_t *) ((i2c_regs[0] << 8) | i2c_regs[1]);  // Update address from message
				sentSize = 0x00;
				update = 0x02;  // Update Display from data at address
				break;
			
			case 0x03:  // Three bytes via I2C
				// Set Digits color
				oneRedColor = i2c_regs[0];
				oneGrnColor = i2c_regs[1];
				oneBluColor = i2c_regs[2];

				tenRedColor = i2c_regs[0];
				tenGrnColor = i2c_regs[1];
				tenBluColor = i2c_regs[2];

				sentSize = 0x00;
				update = 0x00;  // Do not update display
				break;
			
			case 0x04: // Four bytes via I2C
				address = (uint8_t *) ((i2c_regs[0] << 8) | i2c_regs[1]); // Update address from message
				eeprom_write_byte(address, i2c_regs[2]); // Write third byte to EEPROM
				
				sentSize = 0x00;
				update = 0x00;  // Do not update display
				break;

			case 0x05:
				
				for (uint16_t idx = 0; idx < 512; idx++) 
				{
					uint8_t *adr;
					adr = (uint8_t *) idx;
					eeprom_write_byte(adr, i2c_regs[0]);
				}

				sentSize = 0x00;
				update = 0x03;
				break;
			
			case 0x06:  // Six bytes via I2C
				// Set Digits color
				tenRedColor = i2c_regs[0];
				tenGrnColor = i2c_regs[1];
				tenBluColor = i2c_regs[2];

				oneRedColor = i2c_regs[3];
				oneGrnColor = i2c_regs[4];
				oneBluColor = i2c_regs[5];

				sentSize = 0x00;
				update = 0x00; // Do not update display
				break;
		}

		if (display >= 100)
			display %= 100;
		

		if (update)  // update either number or address
		{
			enableLedPin();
			if (update == 0x01) // Write number to EEPROM
				eeprom_write_byte(address, display);
			if (update == 0x02) // Pull number from EEPROM
				display = eeprom_read_byte(address);

			set_digit(display % 10, 0, oneRedColor, oneGrnColor, oneBluColor);  // Set ones place number
			set_digit(display / 10 % 10, 1, tenRedColor, tenGrnColor, tenBluColor);  //Set tens place number
			write_pixels();
			write_pixels();
			update = 0x00;  // do not enter update
			disableLedPin();
		}

		TinyWireS_stop_check();
	}
}
