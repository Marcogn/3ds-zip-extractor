#include "led.h"

// Internal: select an LED notification pattern.
// 0 = off, 1 = green (download + extraction completed),
// 2 = pink/red (download completed, before extraction).
//
// NOTE: actual MCUHWC LED control is not yet wired up here; this preserves
// the historical behaviour from main.c (the previous implementation also
// did not drive the MCU directly). The function signature is kept stable
// so a real implementation can drop in without further refactoring.
static void set_led_notification(unsigned char pattern) {
    (void)pattern;
}

void led_notification_green(void) {
    set_led_notification(1);
}

void led_notification_pink(void) {
    set_led_notification(2);
}

void led_notification_off(void) {
    set_led_notification(0);
}
