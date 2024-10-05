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

#define KP_ROW_PORT   PORTA
#define KP_ROW_DDR    DDRA
#define KP_COL_PORT   PORTB
#define KP_COL_DDR    DDRB
#define KP_COL_PIN    PINB

#define KP_ROWS       2
#define KP_COLS       2

#define KP_FIRST_ROW  0
#define KP_FIRST_COL  0

#define KP_ROW_MASK   0b00000011
#define KP_COL_MASK   0b00000011

void set_pixel(uint8_t i, uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t offset = (i % PIXELS) * 3;
  grb[offset] = SCALE(g);
  grb[offset+1] = SCALE(r);
  grb[offset+2] = SCALE(b);
}

void write_pixels(void) 
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

void clear_pixels(void)
{
	for (uint8_t idx = 0x00; idx < PIXELS*3; idx++) 
	{
		//grb[idx] = SCALE(0x00);
		grb[idx] = 0x00;
	}
		
	write_pixels();
	_delay_ms(10);
}

void set_digit(uint8_t digit, uint8_t pos, uint8_t r, uint8_t g, uint8_t b)
{
	pos = pos * 0x09;
	switch (digit)
	{
		case 0x00:
			set_pixel(pos + 0x00, r, g, b);
			set_pixel(pos + 0x01, r, g, b);
			set_pixel(pos + 0x02, r, g, b);
			set_pixel(pos + 0x03, r, g, b);
			set_pixel(pos + 0x04, r, g, b);
			set_pixel(pos + 0x05, r, g, b);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;
		
		case 0x01:
			set_pixel(pos + 0x00, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x01, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x02, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x03, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x04, r, g, b);
			set_pixel(pos + 0x05, r, g, b);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;
		
		case 0x02:
			set_pixel(pos + 0x00, r, g, b);
			set_pixel(pos + 0x01, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x02, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x03, r, g, b);
			set_pixel(pos + 0x04, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x05, r, g, b);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x08, r, g, b);
			break;
		
		case 0x03:
			set_pixel(pos + 0x00, r, g, b);
			set_pixel(pos + 0x01, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x02, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x03, r, g, b);
			set_pixel(pos + 0x04, r, g, b);
			set_pixel(pos + 0x05, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x06, r, g, b);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		case 0x04:	
			set_pixel(pos + 0x00, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x01, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x02, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x03, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x04, r, g, b);
			set_pixel(pos + 0x05, r, g, b);
			set_pixel(pos + 0x06, r, g, b);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		case 0x05:
			set_pixel(pos + 0x00, r, g, b);
			set_pixel(pos + 0x01, r, g, b);		
			set_pixel(pos + 0x02, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x03, r, g, b);
			set_pixel(pos + 0x04, r, g, b);
			set_pixel(pos + 0x05, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		case 0x06:
			set_pixel(pos + 0x00, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x01, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x02, r, g, b);
			set_pixel(pos + 0x03, r, g, b);
			set_pixel(pos + 0x04, r, g, b);
			set_pixel(pos + 0x05, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x06, r, g, b);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		case 0x07:
			set_pixel(pos + 0x00, r, g, b);			
			set_pixel(pos + 0x01, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x02, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x03, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x04, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x05, r, g, b);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x08, r, g, b);
			break;

		case 0x08:
			set_pixel(pos + 0x00, r, g, b);
			set_pixel(pos + 0x01, r, g, b);
			set_pixel(pos + 0x02, r, g, b);
			set_pixel(pos + 0x03, r, g, b);
			set_pixel(pos + 0x04, r, g, b);
			set_pixel(pos + 0x05, r, g, b);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		case 0x09:
			set_pixel(pos + 0x00, r, g, b);
			set_pixel(pos + 0x01, r, g, b);
			set_pixel(pos + 0x02, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x03, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x04, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x05, r, g, b);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, r, g, b);
			break;

		case 0x0A:
			set_pixel(pos + 0x00, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x01, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x02, r, g, b);
			set_pixel(pos + 0x03, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x04, r, g, b);
			set_pixel(pos + 0x05, r, g, b);
			set_pixel(pos + 0x06, r, g, b);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		case 0x0B:
			set_pixel(pos + 0x00, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x01, r, g, b);
			set_pixel(pos + 0x02, r, g, b);
			set_pixel(pos + 0x03, r, g, b);
			set_pixel(pos + 0x04, r, g, b);
			set_pixel(pos + 0x05, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		case 0x0C:
			set_pixel(pos + 0x00, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x01, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x02, r, g, b);
			set_pixel(pos + 0x03, r, g, b);
			set_pixel(pos + 0x04, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x05, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		case 0x0D:
			set_pixel(pos + 0x00, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x01, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x02, r, g, b);
			set_pixel(pos + 0x03, r, g, b);
			set_pixel(pos + 0x04, r, g, b);
			set_pixel(pos + 0x05, r, g, b);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		case 0x0E:
			set_pixel(pos + 0x00, r, g, b);
			set_pixel(pos + 0x01, r, g, b);
			set_pixel(pos + 0x02, r, g, b);
			set_pixel(pos + 0x03, r, g, b);
			set_pixel(pos + 0x04, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x05, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		case 0x0F:
			set_pixel(pos + 0x00, r, g, b);
			set_pixel(pos + 0x01, r, g, b);
			set_pixel(pos + 0x02, r, g, b);
			set_pixel(pos + 0x03, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x04, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x05, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x06, 0x00, 0x00, 0x00);
			set_pixel(pos + 0x07, r, g, b);
			set_pixel(pos + 0x08, 0x00, 0x00, 0x00);
			break;

		
	}
}

uint8_t scan_keypad(void)
{
	//Return value to button correlation
	//1: Ones place increase
	//2: Ones place decrease
	//3: Tens place increase
	//4: Tens place decrease

	uint8_t val = 0x00;
	uint8_t pos = 0x00;
	uint8_t row_num;
	uint8_t row_bit;
	uint8_t col_num;
	uint8_t col_bit;
	uint8_t cols;

	KP_ROW_PORT |= KP_ROW_MASK;    // set all rows to 1
	row_bit = 1 << KP_FIRST_ROW;    // active-hi bit for current active row

		for (row_num = 0; row_num < KP_ROWS; row_num++)  // check each row in turn
	{
		KP_ROW_PORT &= ~row_bit;      // set next row to 0 ASAP
		_delay_ms(2);
		row_bit <<= 1;
		col_bit = 1 << KP_FIRST_COL;  // bitmask for 1st column

		cols = ~KP_COL_PIN & KP_COL_MASK; // look for any 0 columns (1 after ~)

		KP_ROW_PORT |= KP_ROW_MASK;   // set all rows to 1 ASAP

		for (col_num = 0; col_num < KP_COLS; col_num++)  // check each col for active
		{
			if (cols & col_bit)         // this col in active state, found a keypress
			{
				//val = (row_num * KP_COLS) + col_num + 1;  // +1 to differentiate from no-key
				//return val;
				
				pos = (row_num * KP_COLS) + col_num;  // +1 to differentiate from no-key
				val |= 1 << pos;
				col_bit <<= 1; // next col mask
			}
			else
			{
				col_bit <<= 1; // next col mask
			}
		}
	}    
	return val; // no keypresses found
}

uint8_t filter_keypad_original(void)
{
	static uint8_t last_val = 0;           // 0 means no buttons pushed
	static uint8_t count = 0;              // debounce filter count
	uint8_t val;                           // raw val this time
	uint8_t filtered_val = 0;              // our filtered return val

	val = scan_keypad();              // get raw keypad value

	if (val != 0)                     // have a keypad button push
	{
		if (val == last_val)            // we're filtering this val
		{
			if ((count != 0) && (--count == 0))  // continue filtering & check if done
			{
				filtered_val = val;         // found enough consecutive values to return as valid
			}
		}
		else
		{
			count = 0x08;         // start filtering a new val
		}
	}
	last_val = val;
	return filtered_val;
}

int main(void)
{
	KP_ROW_DDR |= KP_ROW_MASK;
	
	DDRB = 0x00;
	PIXEL_DDR |= (1 << PIXEL_BIT); // Set pixel pin output

	sei(); /* enable interrupts */

	uint8_t button = 0x05;

	while (1)
	{
		/*
		for (uint8_t input = 0x08; input < 0x10; input++) 
		{
			uint8_t low = input % 0x0A;
			uint8_t high = input / 0x0A;

			set_digit(input, 0x00, 0x50, 0x00, 0x00);
			set_digit(input, 0x01, 0x00, 0x50, 0x00);
			write_pixels();
			_delay_ms(2000);
			write_pixels();  // Not sure why this needs to be here

		}
		*/
		/*
		set_digit(0x05, 0x00, 0x00, 0x30, 0x30);
		set_digit(0x04, 0x01, 0x30, 0x30, 0x00);
		write_pixels();
		_delay_ms(1000);

		set_digit(0x00, 0x00, 0x00, 0x30, 0x30);
		set_digit(0x00, 0x01, 0x30, 0x30, 0x00);
		write_pixels();
		_delay_ms(1000);
		*/

		button = filter_keypad_original();

		if (button != 0x00)
		{
			set_digit(button, 0x00, 0x00, 0x00, 0x70);
			write_pixels();
			write_pixels();
			//_delay_ms(1000);
		}
	}
}
