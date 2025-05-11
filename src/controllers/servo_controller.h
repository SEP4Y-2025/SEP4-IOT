// servo_controller.h
#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <stdint.h>

/**
 * @brief Initialize the continuous‐rotation servo into its neutral (stop) position.
 */
void servo_init(void);

/**
 * @brief Directly set the servo to a given pulse “angle” (0–180).
 * @param angle Logical “angle” value; for continuous‐rotation servos, 0 = full CCW, 
 *              90 = stop, 180 = full CW.
 */
void servo_set_angle(uint8_t angle);

/**
 * @brief Rotate the servo one full revolution clockwise.
 */
void rotate_cw_360(void);

/**
 * @brief Rotate the servo one full revolution counter‐clockwise.
 */
void rotate_ccw_360(void);

/**
 * @brief Rotate the servo clockwise at full speed for a given duration, then stop.
 * @param time_ms Duration in milliseconds.
 */
void rotate_cw_time(uint32_t time_ms);

/**
 * @brief Rotate the servo counter‐clockwise at full speed for a given duration, then stop.
 * @param time_ms Duration in milliseconds.
 */
void rotate_ccw_time(uint32_t time_ms);

#endif // SERVO_CONTROLLER_H
