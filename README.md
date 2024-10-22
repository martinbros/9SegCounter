# Nine Segment Counter SAO

**Description**

Use this handy little SAO to count whatever your heart desires: how many conversations you have at Supercon, how many times someone asks what SAO stands for, how many times someone asks "What are you working on?".
By using the onboard buttons, I2C interface, or clocking the GP2 pin, the Nine Segment Counter SAO will indicate to the world whatever quantity you are keeping track of.

## Operation

Store a 2 digit number and display the number using an atypical 9 segment display instead of the typical 7

Using the buttons, a user can store 49 unique numbers in non-volatile memory. Each unique number is identified by the color of the digits.

Using the I2C bus, the user has full read and write access to the ATTiny 84's 512 byte EEPROM and has full command of the display (displayed number and digit color).

- Cap the RGB value

### Button Interface

- Pressing a button above a digit increases that digit by 1
- Pressing a button below a digit decreases that digit by 1
- The displayed digit is stored as a decimal 2 digit number (0-99)
- Pressing the buttons above and below a digit changes the color of that digit
  - Each unique digit color combination corresponds to a unique number saved in non-volatile memory
  - 7 unique colors per "ones" digit x 7 unique colors per "tens" digit = 49 unique values stored in non-volatile memory
  - EEPROM Address location is built by taking the "ones" position color index (0-6) and adding it to the "tens" position color index which has been multipled by 10 (0-6 x 10) [e.g. 00, 02, 03 ... 07, 10, 11 ... 17, 20 ... 27 ... 77]

### I2C Interface

- I2C Address: 4

**Display Functions**

- 1 8-bit message: the display will update to show the sent number

- 3 8-bit messages: set the RGB values of the digits to be displayed (Digit must be refreshed to update color)

- 6 8-bit messages: set the RGB values of the tens place digit and then ones place digit (Digit must be refreshed to update color)

**Memory Functions**

- 2 8-bit message: The sent number is converted to a 16-bit EEPROM address pointer, the display will update to display the value stored in memory.

- 4 8-bit messages: first 2 bytes are combined to a 16-bit EEPROM address pointer, third byte is the data to be stored in the EEPROM, the last byte is a throwaway

### GP1 Interface

- GP1 is normally pulled high. By driving GP1 low, the displayed number will be incremented by 1
- Max clock speed is ~150Hz
- Displayed number is not stored in memory 

## GP2 Interface

- GP2 is connected directly to the first Neopixel and is terminated with a 470 ohm resistor. If you desire, feel free to implement the Neopixel protocol and set the segments however you like! Use the reference photo below for the sequence of the neopixels.