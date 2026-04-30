#ifndef SPEED_METER_H
#define SPEED_METER_H

#include <stdint.h>
#include <stddef.h>

// Rolling-window throughput meter with a fixed-size ring buffer.
// All times are in milliseconds; bytes are cumulative.
//
// The meter keeps the last `SPEED_METER_SAMPLES` samples; throughput is
// computed over a sliding window of `SPEED_METER_WINDOW_MS` (2 seconds).
//
// Pure module — no 3DS dependency, host-testable.

#define SPEED_METER_SAMPLES   16
#define SPEED_METER_WINDOW_MS 2000u

typedef struct {
    uint64_t samples_bytes[SPEED_METER_SAMPLES];
    uint64_t samples_time_ms[SPEED_METER_SAMPLES];
    int head;    // index of the next slot to write
    int filled;  // number of valid samples (0..SPEED_METER_SAMPLES)
} speed_meter_t;

// Reset the meter to the empty state.
void speed_meter_init(speed_meter_t* m);

// Record a (cumulative_bytes, time_ms) sample. Stale samples (older than
// the window) are pruned implicitly when computing the speed.
void speed_meter_sample(speed_meter_t* m, uint64_t cum_bytes, uint64_t now_ms);

// Bytes-per-second over the [now_ms - SPEED_METER_WINDOW_MS, now_ms]
// window. Returns 0.0 when there is not enough data.
double speed_meter_bps(const speed_meter_t* m, uint64_t now_ms);

#endif // SPEED_METER_H
