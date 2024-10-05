# Name: Makefile
# Author: Nathan Witmer
# Copyright: 2015 Nathan Witmer
# License: MIT (see LICENSE)

DEVICE     = attiny84a
AVR_DEVICE = t84
CLOCK      = 8000000
PROGRAMMER = -c avrispmkII
OBJECTS    = mainDigit.o
# 0xe2 for internal 8MHz clock, 0x62 for internal 1MHz:
# 0xdf for SPI enabled, 0xdc to add brown-out at 4.3V
FUSES      = -U lfuse:w:0xe2:m -U hfuse:w:0xdc:m # -U efuse:w:0xff:m

PIXEL_OFFSET = 37

AVRDUDE = avrdude $(PROGRAMMER) -p $(AVR_DEVICE)
COMPILE = avr-gcc -Wall -MMD -Os -g -flto -DF_CPU=$(CLOCK) -DPIXEL_OFFSET=$(PIXEL_OFFSET) -mmcu=$(DEVICE)

all:	build size

run:	build size flash

build:	main.hex

%.o: %.c
	$(COMPILE) -c $< -o $@

%.s: %.c
	$(COMPILE) -S $< -o $@

size:	main.elf
	avr-size --format=avr --mcu=$(DEVICE) main.elf

flash:	main.hex
	$(AVRDUDE) -U flash:w:main.hex:i

check:
	$(AVRDUDE)

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID main.hex

clean:
	rm -f main.hex main.elf $(OBJECTS) *.d

# file targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -l -S -d main.elf | pygmentize -l c-objdump

serial:
	picocom -b 9600 --echo $$(ls /dev/tty.usbserial* | head -1)

include $(wildcard *.d)
