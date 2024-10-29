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

#define redScale 0xFF
#define greenScale 0xFF
#define blueScale 0xFF
uint8_t colors[][3] = {{0x00, 0x00, blueScale},
						{0x00, greenScale, 0x00},
						{0x00, greenScale, blueScale},
						{redScale, 0x00, 0x00},
						{redScale, 0x00, blueScale},
						{redScale, greenScale, 0x00},
						{redScale, greenScale, blueScale},
						{0, 0, 0}};

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

const uint8_t HSVlights[61] = 
{0, 4, 8, 13, 17, 21, 25, 30, 34, 38, 42, 47, 51, 55, 59, 64, 68, 72, 76,
81, 85, 89, 93, 98, 102, 106, 110, 115, 119, 123, 127, 132, 136, 140, 144,
149, 153, 157, 161, 166, 170, 174, 178, 183, 187, 191, 195, 200, 204, 208,
212, 217, 221, 225, 229, 234, 238, 242, 246, 251, 255};

uint16_t colorWheel = 0;
uint8_t rgbOne[3] = {0};
uint8_t rgbTen[3] = {0};

uint16_t clockCount = 0;

void trueHSV(uint16_t angle, uint8_t *rgbArray)
{
  uint8_t red, green, blue;

  angle %= 360;

  if (angle<60) {red = 255; green = HSVlights[angle]; blue = 0;} else
  if (angle<120) {red = HSVlights[120-angle]; green = 255; blue = 0;} else 
  if (angle<180) {red = 0, green = 255; blue = HSVlights[angle-120];} else 
  if (angle<240) {red = 0, green = HSVlights[240-angle]; blue = 255;} else 
  if (angle<300) {red = HSVlights[angle-240], green = 0; blue = 255;} else 
                 {red = 255, green = 0; blue = HSVlights[360-angle];}

  rgbArray[0] = red;
  rgbArray[1] = green;
  rgbArray[2] = blue;
}


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
	
	rgbOne[0] = colors[oneColorIdx][0];
	rgbOne[1] = colors[oneColorIdx][1];
	rgbOne[2] = colors[oneColorIdx][2];
	rgbTen[0] = colors[oneColorIdx][0];
	rgbTen[1] = colors[oneColorIdx][1];
	rgbTen[2] = colors[oneColorIdx][2];

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
				oneColorIdx %= 8;

				rgbOne[0] = colors[oneColorIdx][0];
				rgbOne[1] = colors[oneColorIdx][1];
				rgbOne[2] = colors[oneColorIdx][2];

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
				tenColorIdx %= 8;

				rgbTen[0] = colors[tenColorIdx][0];
				rgbTen[1] = colors[tenColorIdx][1];
				rgbTen[2] = colors[tenColorIdx][2];

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
				rgbOne[0] = i2c_regs[0];
				rgbOne[1] = i2c_regs[1];
				rgbOne[2] = i2c_regs[2];

				rgbTen[0] = i2c_regs[0];
				rgbTen[1] = i2c_regs[1];
				rgbTen[2] = i2c_regs[2];

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
				rgbTen[0] = i2c_regs[0];
				rgbTen[1] = i2c_regs[1];
				rgbTen[2] = i2c_regs[2];

				rgbOne[0] = i2c_regs[3];
				rgbOne[1] = i2c_regs[4];
				rgbOne[2] = i2c_regs[5];

				sentSize = 0x00;
				update = 0x00; // Do not update display
				break;
		}

		if (oneColorIdx == 7 && tenColorIdx == 7)
		{
			clockCount++;
			if (clockCount > 8)
			{
				display++;
				clockCount = 0;
			}
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

			if (oneColorIdx == 7)
				trueHSV(colorWheel, rgbOne);

			if (tenColorIdx == 7) 
				trueHSV(colorWheel + 45, rgbTen);

			set_digit(display % 10, 0, rgbOne[0], rgbOne[1], rgbOne[2]);  // Set ones place number
			set_digit(display / 10 % 10, 1, rgbTen[0], rgbTen[1], rgbTen[2]);  //Set tens place number
			write_pixels();
			write_pixels();

			if (oneColorIdx == 7 || tenColorIdx == 7)
			{
				colorWheel++;
				if (colorWheel > 359)
					colorWheel = 0;
				
				_delay_ms(10);
				update = 0x03;
			}

			else
				update = 0x00;  // do not enter update

			
			disableLedPin();
		}

		TinyWireS_stop_check();
	}
}
