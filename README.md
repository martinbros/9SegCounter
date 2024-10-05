# lightbar

## What is?

* Firmware for an attiny84 to drive a 16-pixel neopixel bar via softuart serial control.
* A ruby script to talk to the serial port and display colors and animations.

## Why for?

So I can make pretty colors.

## Is it any good?

Yes.

## How do?

### Firmware

Dependencies:

* `brew install avrdude`
* `brew tap osx-cross/avr`
* `brew install avr-libc`

```
cd firmware
make flash
```

### Circuit

#### Parts

* ATTiny84, programmed as above
* Two [neopixel sticks](https://www.adafruit.com/product/1426) soldered together. If you've got something else, edit the firmware/ruby script to change the number of pixels accordingly.
* A big ol' electrolytic capacitor (100uF+) for power conditioning
* A 470Ω resistor
* A [5V FTDI breakout](https://www.sparkfun.com/products/9716) and USB cable, for power and comms.

#### Assembly

- [ ] Capacitor bridges 5V and GND, near the pixel stick.
- [ ] FTDI TX goes to pin 12
- [ ] FTDI RX goes to pin 11
- [ ] FTDI 5V and GND for power
- [ ] Pin 8 goes to resistor, goes to pixel stick data line

The firmware restricts total max rgb output levels to 127, which (give or take) will prevent 16 pixels from consuming more than 500mA to keep USB hosts happy. If you want more light or are driving more pixels, don't rely on USB for power.

### Blinkenlights

Plug in the FTDI breakout and you should have a `/dev/cu.usbserial-*` device.

For a preset animation:

```
cd controller
ruby animation.rb
```

For console shenanigans:

```
cd controller
irb -r./animation

> # helper for easy hex input:
> wipe "0f0"
> wipe "ff0088"
> loop { wipe gets.strip }
> # or control manually:
> strip.rainbow_cycle
> # etc.
```