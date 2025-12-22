#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <stdint.h>

typedef struct {
    uint16_t left;
    uint16_t front;
    uint16_t right;
} nav_scan_t;

extern volatile uint8_t nav_scan_request;

volatile uint16_t ultrasonic_read_cm_blocking(void);
void navigation_init(void);
void navigation_scan(nav_scan_t *scan);
uint8_t navigation_is_finish(void);

#endif
