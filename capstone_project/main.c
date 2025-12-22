/*
|=======================================================================================|
|   Name: Andrey Fritz L. Solijon                                                       |    
|   Capstone Proj. Title: Maze Solver and Delivery Robot using Bare-Metal C Programming |
|   [In compliance with the final reqiurement for COE185 (Embedded Systems)]            |
|=======================================================================================|
*/

#include "motor.h"
#include "sensor.h"
#include "uart.h"
#include "servo.h"
#include "navigation.h"
#include "fsm.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

int main(void) {
    uart_init();
    stdout = uart_get_stream(); // Redirect stdout to our UART stream
    motor_init();
    sensor_init();
    servo_init();
    navigation_init();
    fsm_init();
    sei();

    nav_scan_t scan;

    printf("Delivery Maze-Solving Robot Initialized!\n");

    while(1) {
        scan.front = ultrasonic_read_cm_blocking();
        
        fsm_update(&scan);
        printf("State: %d\tLeft: %u cm\tFront: %u cm\tRight: %u cm\n", fsm_get_state(), scan.left, scan.front, scan.right);
    }
}