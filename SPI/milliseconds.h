#ifndef MILLISECONDS_H
#define MILLISECONDS_H

#include <stdint.h>

void setup_timer(void);
void delay(uint64_t value);
uint64_t get_milliseconds(void);

#endif
