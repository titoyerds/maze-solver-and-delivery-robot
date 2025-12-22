#ifndef DECIDE_H
#define DECIDE_H

#include <stdint.h>
#include "navigation.h"

typedef enum {
    DIR_FRONT = 0,
    DIR_RIGHT = 1,
    DIR_BACK  = 2,
    DIR_LEFT  = 3
} direction_t;

void decide_init(void);
direction_t direction_decide(nav_scan_t *scan);

#endif