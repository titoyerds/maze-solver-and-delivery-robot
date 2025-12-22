#ifndef FSM_H
#define FSM_H

#include "navigation.h"

// --- FSM and Robot Parameters ---
typedef enum {
    WAITING_PAYLOAD,
    MOVE_FORWARD,
    STOP,
    TURN_LEFT,
    TURN_RIGHT,
    U_TURN,
    EMERGENCY_STOP
} RobotState;

void fsm_init(void);
void fsm_update(nav_scan_t *scan);

RobotState fsm_get_state(void);
void fsm_set_state(RobotState new_state);

void timer_init(void);
uint32_t millis(void);

#endif
