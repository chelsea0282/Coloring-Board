#ifndef TFT_H
#define TFT_H

#include <stdint.h>

#ifndef TFT_CLK
#	define TFT_CLK PORTE, PTE, 26
#endif
#ifndef TFT_MISO
#	define TFT_MISO PORTD, PTD, 2
#endif
#ifndef TFT_MOSI
#	define TFT_MOSI PORTD, PTD, 0
#endif
#ifndef TFT_CS
#	define TFT_CS PORTB, PTB, 19
#endif
#ifndef TFT_DC
#	define TFT_DC PORTB, PTB, 18
#endif
#ifndef TFT_RST
#	define TFT_RST PORTB, PTB, 23
#endif

void tft_begin(void);
uint8_t tft_read_command(uint8_t command);
void tft_fill_screen(int x, int y, int w, int h, int c);

#endif
