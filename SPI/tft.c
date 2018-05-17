#include "tft.h"
#include <fsl_device_registers.h>
//#include "MK64F12.h"
#include "milliseconds.h"

/* Lower Level Function */

// Set pin as input
static inline void set_input(PORT_Type* port, GPIO_Type* gpio, int pin) {
  // Enable GPIO
  port->PCR[pin] = (1 <<  8);
  // Set as output
  gpio->PDDR &= ~(1 << pin);
  // Set output as low
  gpio->PCOR |= 1 << pin;
}

// Set pin as output
static inline void set_output(PORT_Type* port, GPIO_Type* gpio, uint8_t pin) {
  // Enable GPIO
  port->PCR[pin] = (1 <<  8);
  // Set as output
  gpio->PDDR |= 1 << pin;
}

// Set output pin to high
static inline void set_high(PORT_Type* port, GPIO_Type* gpio, uint8_t pin) {
  gpio->PSOR |= 1 << pin;
}

// Set output pin to low
static inline void set_low(PORT_Type* port, GPIO_Type* gpio, uint8_t pin) {
  gpio->PCOR |= 1 << pin;
}

// Get input from pin
static inline uint8_t get(PORT_Type* port, GPIO_Type* gpio, uint8_t pin) {
  return (gpio->PDIR >> pin) & 0x1;
}

/* End Lower Level Functions */


/* Operational Functions */

// Start writing or reading to the TFT
static void open() {
  set_low(TFT_CS);
  set_low(TFT_CLK);
  set_low(TFT_MOSI);
  set_low(TFT_DC);
}

// Stop writing to the TFT
static void close() {
  set_high(TFT_CS);
}

// Reset the TFT
static void reset() {
  set_low(TFT_RST);
  delay(500);
  set_high(TFT_RST);
  delay(500);
}

/* End Operational Functions */


/* SPI Helper Functions */

// Write a byte of data
static void write(uint8_t value) {
  for (uint8_t bit = 0x80; bit > 0; bit >>= 1) {
    if ((value & bit) == bit) {
      set_high(TFT_MOSI);
    }
    else {
      set_low(TFT_MOSI);
    }
    set_low(TFT_CLK);
    set_high(TFT_CLK);
  }
}

// Write 2 bytes of data
static inline void write16(uint16_t value) {
  write((value >> 8) & 0xFF);
  write((value >> 0) & 0xFF);
}

// Write 4 bytes of data
static inline void write32(uint32_t value) {
  write((value >> 24) & 0xFF);
  write((value >> 16) & 0xFF);
  write((value >> 8) & 0xFF);
  write((value >> 0) & 0xFF);
}

// Read a byte of data
static uint8_t read() {
  uint8_t r = 0;
  for (uint8_t i = 0; i < 8; i++) {
    set_low(TFT_CLK);
    set_high(TFT_CLK);
    r = r << 1;
    r = r | get(TFT_MISO);
  }
  return r;
}

// Write a command
static void write_command(uint8_t cmd) {
  set_low(TFT_DC);
  write(cmd);
  set_high(TFT_DC);
}

// Send a command and data
static void send(uint8_t command, uint32_t data_len, uint8_t* data) {
  write_command(command);
  for (uint32_t i = 0; i < data_len; i++) {
    write(data[i]);
  }
}

/* End SPI Helper Functions */


/* TFT Protocol Functions */

static void setAddrWindow(int x, int y, int w, int h) {
  uint32_t xa = (x << 16) | (x + w - 1);
  uint32_t ya = (y << 16) | (y + h - 1);
  write_command(0x2A); // ILI9341_CASET
  write32(xa);
  write_command(0x2B); // ILI9341_PASET
  write32(ya);
  write_command(0x2C); // ILI9341_RAMWR
}

static void writeColor(uint16_t c, uint32_t l) {
  while (l-- > 0) {
    write16(c);
  }
}

/* End TFT Protocol Function */


/* Exposed TFT Functions */

//srets up the TFT board to be able to transmit pictures
void tft_begin() {
  // Enable ports B and E
  SIM->SCGC5 |= (1 <<  10) | (1 <<  13) |(1 <<  12)|(1 <<  11);

  set_output(TFT_CLK);
  set_output(TFT_MOSI);
  set_input(TFT_MISO);
  set_output(TFT_CS);
  set_output(TFT_DC);
  set_output(TFT_RST);

  reset();

  open();

  send(0xEF, 3, (uint8_t[]) {0x03, 0x80, 0x02});
  send(0xCF, 3, (uint8_t[]) {0x00, 0xC1, 0x30});
  send(0xED, 4, (uint8_t[]) {0x64, 0x03, 0x12, 0x81});
  send(0xE8, 3, (uint8_t[]) {0x85, 0x00, 0x78});
  send(0xCB, 5, (uint8_t[]) {0x39, 0x2C, 0x00, 0x34, 0x02});
  send(0xF7, 1, (uint8_t[]) {0x20});
  send(0xEA, 2, (uint8_t[]) {0x00, 0x00});

  send(0xC0, 1, (uint8_t[]) {0x23}); // Power control 1
  send(0xC1, 1, (uint8_t[]) {0x10}); // Power control 2
  send(0xC5, 2, (uint8_t[]) {0x3E, 0x28}); // VCM control 1
  send(0xC7, 1, (uint8_t[]) {0x86}); // VCM control 2
  send(0x36, 1, (uint8_t[]) {0x48}); // Memory access control
  send(0x37, 2, (uint8_t[]) {0x00, 0x00}); // Vertical scroll

  send(0x3A, 1, (uint8_t[]) {0x55}); // ILI9341_PIXFMT
  send(0xB1, 2, (uint8_t[]) {0x00, 0x18}); // ILI9341_FRMCTR1
  send(0xB6, 3, (uint8_t[]) {0x08, 0x82, 0x27}); // ILI9341_DFUNCTR
  send(0xF2, 1, (uint8_t[]) {0x00});
  send(0x26, 1, (uint8_t[]) {0x01}); // ILI9341_GAMMASET
  send(0xE0, 15, (uint8_t[]) {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00}); // ILI9341_GMCTRP1
  send(0xE1, 15, (uint8_t[]) {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F}); // ILI9341_GMCTRN1

  write_command(0x11); // Sleep out
  delay(120);

  write_command(0x29); // Display on
  delay(120);

  close();
}

//reads the status of whiting an image to the TFT
uint8_t tft_read_command(uint8_t command) {
  open();
  send(0xD9, 1, (uint8_t[]) {0x10});
  write_command(command);
  uint8_t r = read();
  close();
  return r;
}

//Creates a rectangle of width w and height h at position
//x,y and fills it with color c
void tft_fill_screen(int x, int y, int w, int h, int c) {
  open();

  setAddrWindow(x, y, w, h);
  int l = w * h;
  writeColor(c, l);

  close();
}

/* End Exposed TFT Functions */
