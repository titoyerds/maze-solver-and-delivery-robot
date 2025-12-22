#include "decide.h"
#include <stdio.h>

#define WALL_DIST_CM 25

static uint8_t is_open(uint16_t d)
{
    return (d > WALL_DIST_CM);
}

direction_t direction_decide(nav_scan_t *scan)
{
    uint8_t left  = is_open(scan->left);
    uint8_t front  = is_open(scan->front);
    uint8_t right = is_open(scan->right);

    if (left && !right) return DIR_LEFT;
    if (right && !left) return DIR_RIGHT;

    if (left && right) {
        // Both sides open: Pick the wider/deeper one
        if (scan->left > scan->right) return DIR_LEFT;
        else return DIR_RIGHT;
    }

    if (front) {
        return DIR_FRONT;
    }

    if (!left && !right) {
        return DIR_BACK;
    }
}
