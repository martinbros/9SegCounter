# Nine Segment Counter SAO

** Description **

Use this handy little SAO to count whatever your heart desires: how many conversations you have at supercon, how many times someone asks what SAO stands for, how many times someone asks "What are you working on?".
By using the onboard buttons, I2C interface, or clocking the GP2 pin, the Nine Segment Counter SAO will indicate to the world whatever quantity you are keeping track of.

## Operation

Store a 2 digit number and displays the number using an atypical 9 segment display instead of the typical 7

- Perhaps store a number depending on color
	- store a single digit number depending on color regardless of digit position
	- If both digits are the same color, store the 2 digit number
- Store numbers in EEPROM
- Cap the RGB value

### Button Interface

- Pressing a button above a digit increases that digit by 1
- Pressing a button below a digit decreases that digit by 1
- Pressing the buttons above and below a digit changes the color of that digit

### I2C Interface

- Address is XXX
- 1 8-bit message: the display will update to show the sent number
- 4 8-bit messages: the first message will be the displayed number, the subsequent 3 messages will be the RGB value of the displayed digits
- 7 8-bit messages: the first message will be the displayed number, the subsequent 6 messages will be the RGB values of the first and then second digit

### GP2 Interface

- GP2 is normally pulled high. By driving GP2 low, the displayed number will be incremented by 1
- Maximum clock speed is XXXX 

## GP1 Interface

- GP1 is connected directly to the first Neopixel and is terminated with a 470 ohm resistor. If you desire, feel free to implement the Neopixel protocol and set the segments however you like! Use the reference photo below for the sequence of the neopixels.