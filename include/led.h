#ifndef LED_H
#define LED_H

#ifndef HOST_BUILD
#include <3ds.h>
#endif

// LED notification helpers.
//
// On real 3DS hardware these will eventually drive the MCUHWC LED. The
// current implementation is a thin abstraction matching the historical
// behaviour: the pattern argument selects a colour family (1=green,
// 2=pink/red, 0=off). Centralising the calls here keeps the rest of
// the codebase free from MCU-specific details and makes it easy to
// plug in a real implementation later.
void led_notification_green(void);
void led_notification_pink(void);
void led_notification_off(void);

#endif // LED_H
