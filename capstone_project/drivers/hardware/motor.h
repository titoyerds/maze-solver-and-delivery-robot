#include <stdint.h>
#ifndef MOTOR_H
#define MOTOR_H
void motor_init(void);
void motor_forward(void);
void motor_backward(void);
void motor_turn_left(void);
void motor_turn_right(void);
void motor_stop(void);
void motor_set_speed(uint8_t speed);
#endif