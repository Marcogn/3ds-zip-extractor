#ifndef ETA_FORMAT_H
#define ETA_FORMAT_H

#include <stddef.h>

// Format a remaining-time estimate as `mm:ss`. Pure module, host-testable.
//
// - `seconds < 0` or unknown      → "--:--"
// - `seconds > 99 * 60 + 59`      → "99:59" (capped, never overflows)
// - `out` must point to a buffer of at least 6 bytes (5 chars + NUL).
//   If `n < 6`, the buffer is left as an empty string.
void format_eta(int seconds, char* out, size_t n);

#endif // ETA_FORMAT_H
