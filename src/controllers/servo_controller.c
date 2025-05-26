// // servo_controller.c
// #include <stdint.h>
// #include <util/delay.h>
// #include "servo.h"
// #include "includes.h"        // for F_CPU, etc.

// /// Time (ms) it takes your continuous-rotation servo
// /// to do exactly one full 360° turn at “full speed”.
// /// Measure this empirically and tweak as needed.
// #ifndef FULL_ROTATION_TIME_MS
//   #define FULL_ROTATION_TIME_MS 1000
// #endif

// /// Initialize servo (puts continuous‐rotation servo into neutral/stop)
// void servo_init(void) {
//     // 90µs ≈ neutral on most continuous-rotation servos
//     servo(90);
//     _delay_ms(500);
// }

// /// Directly set servo pulse to a given “angle” (0–180)
// void servo_set_angle(uint8_t angle) {
//     servo(angle);
// }

// /// Rotate one full revolution clockwise
// void rotate_cw_360(void) {
//     rotate_cw_time(FULL_ROTATION_TIME_MS);
// }

// /// Rotate one full revolution counter-clockwise
// void rotate_ccw_360(void) {
//     rotate_ccw_time(FULL_ROTATION_TIME_MS);
// }

// /// Run at full CW speed for exactly time_ms milliseconds, then stop
// void rotate_cw_time(uint32_t time_ms) {
//     // full CW
//     servo(180);

//     // run a loop of 1 ms delays
//     for (uint32_t i = 0; i < time_ms; ++i) {
//         _delay_ms(1);  // 1 is a compile-time constant
//     }

//     // stop
//     servo(90);
// }

// void rotate_ccw_time(uint32_t time_ms) {
//     // full CCW
//     servo(0);

//     for (uint32_t i = 0; i < time_ms; ++i) {
//         _delay_ms(1);
//     }

//     servo(90);
// }
