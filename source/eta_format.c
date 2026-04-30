#include "eta_format.h"

#include <stdio.h>

#define ETA_CAP_SECONDS (99 * 60 + 59)

void format_eta(int seconds, char* out, size_t n) {
    if (!out || n == 0) return;
    if (n < 6) { out[0] = '\0'; return; }

    if (seconds < 0) {
        // unknown
        out[0] = '-'; out[1] = '-'; out[2] = ':';
        out[3] = '-'; out[4] = '-'; out[5] = '\0';
        return;
    }
    if (seconds > ETA_CAP_SECONDS) seconds = ETA_CAP_SECONDS;

    int mm = seconds / 60;
    int ss = seconds % 60;
    snprintf(out, n, "%02d:%02d", mm, ss);
}
