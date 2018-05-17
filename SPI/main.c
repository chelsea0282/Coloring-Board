#include "MK64F12.h"
#include "milliseconds.h"
#include "tft.h"

int main() {
  setup_timer();
  tft_begin();
  delay(1000);
  int result;
  result = tft_read_command(0x0A); // ILI9341_RDMODE
  result = tft_read_command(0x0B); // ILI9341_RDMADCTL
  result = tft_read_command(0x0C); // ILI9341_RDPIXFMT
  result = tft_read_command(0x0D); // ILI9341_RDIMGFMT
  result = tft_read_command(0x0F); // ILI9341_RDSELFDIAG

  tft_fill_screen(100, 100, 100, 100, 0xF800); // Red square
  return 0;
}
