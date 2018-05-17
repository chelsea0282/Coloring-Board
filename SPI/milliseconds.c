#include "milliseconds.h"
#include "MK64F12.h"

volatile uint64_t counter;

// Setup millisecond timer
void setup_timer() {
  // Initialize PIT timer
  SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
  PIT_MCR = 0x0;

  // If the timer is already enabled, turn it off first
  if (PIT->CHANNEL[0].TCTRL == 0x3) {
    PIT->CHANNEL[0].TCTRL = 0x1;
  }

  // 20970 cycles per millisecond
  PIT->CHANNEL[0].LDVAL = 20970;
  PIT->CHANNEL[0].TCTRL = 0x3;

  NVIC_EnableIRQ(PIT0_IRQn);
}

void delay(uint64_t value) {
  uint64_t start = get_milliseconds();
  while (get_milliseconds() - start < value);
}

uint64_t get_milliseconds() {
  return counter;
}

void PIT0_IRQHandler(void) {
  // Clear
  PIT->CHANNEL[0].TFLG |= 1;
  // Disable
  PIT->CHANNEL[0].TCTRL &= ~3;
  // Enable
  PIT->CHANNEL[0].TCTRL |= 3;

  counter++;
}
