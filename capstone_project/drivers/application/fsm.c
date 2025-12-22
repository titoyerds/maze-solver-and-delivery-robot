#include "fsm.h"
#include "motor.h"
#include "decide.h"
#include "navigation.h"
#include "sensor.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include <util\delay.h>

#define MOTOR_SPEED 65 // Speed from 0-255
#define FRONT_DISTANCE_THRESHOLD 15

#define TIME_REVERSE  400
#define TIME_TURN     800
#define TIME_UTURN    1600
#define TIME_PAYLOAD  3000

// Make currentState a volatile global so the ISR can modify it.
volatile RobotState currentState;
volatile uint32_t system_millis = 0; // The "Clock"

// Timer variables to track duration
static uint32_t state_start_time = 0;
static uint32_t payload_verify_start_time = 0;

// --- TIMER SYSTEM (The Heart of Non-Blocking) ---
// We use Timer0 Overflow to count milliseconds (approx)
// If you use Timer0 for Motors, this safely piggybacks on it.
void timer_init(void) {
    // Enable Timer0 Overflow Interrupt
    TIMSK0 |= (1 << TOIE0);
    
    // Ensure Timer0 is running (Prescaler 64 is common for PWM)
    // If motors aren't initing this, uncomment below:
    // TCCR0B |= (1 << CS01) | (1 << CS00); 
}


RobotState fsm_get_state(void) {
    return currentState;
}

void fsm_set_state(RobotState new_state) {
    currentState = new_state;
    state_start_time = millis(); // Reset timer for the new state
}

// ISR runs every ~1ms (depending on prescaler)
ISR(TIMER0_OVF_vect) {
    // Increment system time
    // With Prescaler 64 @ 16MHz, OVF happens every 1.024ms. Close enough.
    system_millis++;
}

uint32_t millis(void) {
    uint32_t m;
    // Atomic read of 32-bit variable
    cli();
    m = system_millis;
    sei();
    return m;
}

void fsm_init(void) {
    timer_init(); // Start the clock
    currentState = WAITING_PAYLOAD;
    state_start_time = millis();
}

void fsm_update(nav_scan_t *scan)
{
    uint32_t now = millis();

    // If we are moving but payload is gone -> STOP
    if (currentState != WAITING_PAYLOAD && currentState != EMERGENCY_STOP) {
        if (payload_change_detected) {
             payload_change_detected = 0; // Clear flag
             if (!sensor_read_payload()) {
                 // Double check (Debounce) using non-blocking timer would be better, 
                 // but for emergency, a quick check is okay.
                 printf("EMERGENCY: Payload Lost!\n");
                 motor_stop();

                 // BLOCKING VERIFICATION LOOP
                 // We wait here for 3 seconds. The rest of the robot freezes.
                 uint8_t lost_confirmed = 1;
                 
                 for (int i = 0; i < 30; i++) { // 30 * 100ms = 3000ms (3s)
                     _delay_ms(100);
                     
                     // If sensor sees the payload again, abort immediately
                     if (sensor_read_payload()) {
                         printf("False alarm! Payload recovered.\n");
                         lost_confirmed = 0;
                         break; 
                     }
                 }
                 if (lost_confirmed) {
                    printf("PAYLOAD CONFIRMED LOST. Executing U-Turn.\n");
                    fsm_set_state(U_TURN); // Change state to U-Turn
                 }
                 return;
             }
        }
    }

    switch (currentState) {
        case WAITING_PAYLOAD:
            motor_stop();

            // Non-Blocking Payload Verification
            if (sensor_read_payload()) {
                // If this is the first time we see it, verify_start_time is 0
                if (payload_verify_start_time == 0) {
                    payload_verify_start_time = now;
                    printf("Payload detected... verifying...\n");
                }
                
                // Check if 3 seconds have passed
                if ((now - payload_verify_start_time) >= TIME_PAYLOAD) {
                    printf("PAYLOAD CONFIRMED. GO!\n");
                    payload_verify_start_time = 0; // Reset
                    fsm_set_state(MOVE_FORWARD);
                }
            } else {
                // Reset timer if payload disappears
                if (payload_verify_start_time != 0) {
                    printf("Payload removed early.\n");
                    payload_verify_start_time = 0;
                }
            }
            break;

        case MOVE_FORWARD:
            motor_set_speed(MOTOR_SPEED);
            motor_forward();

            if (scan->front <= FRONT_DISTANCE_THRESHOLD) {
                fsm_set_state(STOP);
            }
            break;

        case STOP:
            motor_set_speed(MOTOR_SPEED);
            motor_backward();
            // Wait for 400ms using Timer
            if ((now - state_start_time) >= TIME_REVERSE) {
                motor_stop();
                
                // Kick off the scan (Non-blocking start)
                navigation_scan(scan); // Temporarily blocking call
                            
                // Since we used blocking scan above, jump straight to decide
                direction_t dir = direction_decide(scan);
                printf("Turning %u\n", dir);
                if (dir == DIR_LEFT) fsm_set_state(TURN_LEFT);
                else if (dir == DIR_RIGHT) fsm_set_state(TURN_RIGHT);
                else if (dir == DIR_BACK) fsm_set_state(U_TURN);
                else fsm_set_state(MOVE_FORWARD);
            }
            break;

        case TURN_LEFT:
            motor_set_speed(MOTOR_SPEED);
            motor_turn_left();

            if ((now - state_start_time) >= TIME_TURN) {
                fsm_set_state(MOVE_FORWARD);
            }
            break;

        case TURN_RIGHT:
            motor_set_speed(MOTOR_SPEED);
            motor_turn_right();

            if ((now - state_start_time) >= TIME_TURN) {
                fsm_set_state(MOVE_FORWARD);
            }
            break;

        case U_TURN:
            motor_set_speed(MOTOR_SPEED);
            motor_turn_left();

            if ((now - state_start_time) >= TIME_UTURN) {
                fsm_set_state(MOVE_FORWARD);
            }
            break;

        case EMERGENCY_STOP:
            motor_stop();
            break;
    }
}