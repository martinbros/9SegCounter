# Nine Segment Counter SAO

**Description**

Use this handy little SAO to count whatever your heart desires: how many conversations you have at Supercon, how many times someone asks what SAO stands for, how many times someone asks "What are you working on?".
By using the onboard buttons, I2C interface, or clocking the GP2 pin, the Nine Segment Counter SAO will indicate to the world whatever quantity you are keeping track of.

## Operation

Store a 2 digit number and display the number using an atypical 9 segment display instead of the typical 7

Using the buttons, a user can store 49 unique numbers in non-volatile memory. Each unique number is identified by the color of the digits.

- Cap the RGB value

### Button Interface

- Pressing a button above a digit increases that digit by 1
- Pressing a button below a digit decreases that digit by 1
- The displayed digit is stored as a decimal 2 digit number (0-99)
- Pressing the buttons above and below a digit changes the color of that digit
  - Each unique digit color combination corresponds to a unique number saved in non-volatile memory
  - 7 unique colors per "ones" digit x 7 unique colors per "tens" digit = 49 unique values stores in non-volatile memory
  - EEPROM Address location is built by taking the "ones" position color index (0-6) and adding it to the "tens" position color index which has been multipled by 10 (0-6 x 10) [e.g. 00, 02, 03 ... 07, 10, 11 ... 17, 20 ... 27 ... 77]

### I2C Interface

- Address is XXX

- 1 8-bit message: the display will update to show the sent number

- 3 8-bit messages: set the RGB values of the digits to be displayed

- 6 8-bit messages: set the RGB values of the tens place digit and then ones place digit to be displayed


- 2 8-bit message: Address pointer from which the display will pull the data stored in EEPROM and update the display to show that number

- 4 8-bit messages: first 2 bytes is a memory location, third byte is the data to be stored, the last is a throwaway

### GP1 Interface

- GP1 is normally pulled high. By driving GP1 low, the displayed number will be incremented by 1
- Max clock speed is ~150Hz
- Displayed number is not stored in memory 

## GP2 Interface

- GP2 is connected directly to the first Neopixel and is terminated with a 470 ohm resistor. If you desire, feel free to implement the Neopixel protocol and set the segments however you like! Use the reference photo below for the sequence of the neopixels.