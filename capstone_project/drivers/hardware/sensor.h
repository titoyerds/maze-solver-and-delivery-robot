#ifndef SENSOR_H
#define SENSOR_H
#include <stdint.h>

// 1 = Check needed, 0 = No change
extern volatile uint8_t payload_change_detected;

void sensor_init(void);
void sensor_trigger(void);
uint8_t sensor_is_measurement_complete(void);
uint16_t sensor_get_pulse_width(void);
uint8_t sensor_read_payload(void);
#endif