#ifndef NineSegSao_h
#define NineSegSao_h

#include <inttypes.h>

void set_pixel(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
void write_pixels(void);
void clear_pixels(void);
void set_digit(uint8_t digit, uint8_t pos, uint8_t r, uint8_t g, uint8_t b);
uint8_t scan_keypad(void);
uint8_t filter_keypad_original(void);
uint8_t initializeIO(void);

#endif