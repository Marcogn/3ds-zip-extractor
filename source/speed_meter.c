#include "speed_meter.h"

#include <string.h>

void speed_meter_init(speed_meter_t* m) {
    if (!m) return;
    memset(m, 0, sizeof(*m));
}

void speed_meter_sample(speed_meter_t* m, uint64_t cum_bytes, uint64_t now_ms) {
    if (!m) return;
    m->samples_bytes[m->head] = cum_bytes;
    m->samples_time_ms[m->head] = now_ms;
    m->head = (m->head + 1) % SPEED_METER_SAMPLES;
    if (m->filled < SPEED_METER_SAMPLES) m->filled++;
}

// Find the oldest sample inside the window [now_ms - WINDOW, now_ms] and
// the newest sample (which is the latest one we recorded, i.e. the slot
// just before `head`). Returns 0 if there are fewer than two usable
// samples in the window.
double speed_meter_bps(const speed_meter_t* m, uint64_t now_ms) {
    if (!m || m->filled < 2) return 0.0;

    // Latest sample = slot just before head.
    int latest = (m->head - 1 + SPEED_METER_SAMPLES) % SPEED_METER_SAMPLES;
    uint64_t latest_t = m->samples_time_ms[latest];
    uint64_t latest_b = m->samples_bytes[latest];

    // Window cutoff (clamped: don't underflow if now_ms < window).
    uint64_t cutoff = (now_ms > SPEED_METER_WINDOW_MS)
                    ? (now_ms - SPEED_METER_WINDOW_MS)
                    : 0;

    // Walk samples from oldest to newest looking for the first one inside
    // the window.
    int count = m->filled;
    int oldest_idx = (m->head - count + SPEED_METER_SAMPLES) % SPEED_METER_SAMPLES;

    int chosen = -1;
    for (int i = 0; i < count; i++) {
        int idx = (oldest_idx + i) % SPEED_METER_SAMPLES;
        if (m->samples_time_ms[idx] >= cutoff) {
            chosen = idx;
            break;
        }
    }
    if (chosen < 0 || chosen == latest) return 0.0;

    uint64_t old_t = m->samples_time_ms[chosen];
    uint64_t old_b = m->samples_bytes[chosen];

    if (latest_t <= old_t) return 0.0;
    if (latest_b < old_b) return 0.0;  // monotonic counter expected

    double dt = (double)(latest_t - old_t) / 1000.0;       // seconds
    double db = (double)(latest_b - old_b);                // bytes
    if (dt <= 0.0) return 0.0;
    return db / dt;
}
