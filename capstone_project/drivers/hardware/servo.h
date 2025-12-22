#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

void servo_init(void);
void servo_set_angle(uint8_t angle);
void servo_update(void);

#endif
