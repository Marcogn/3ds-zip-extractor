// Unit tests for the rolling speed meter.
#include "unity.h"
#include "../include/speed_meter.h"

#include <math.h>

void setUp(void) {}
void tearDown(void) {}

static void test_zero_samples_is_zero_bps(void) {
    speed_meter_t m;
    speed_meter_init(&m);
    TEST_ASSERT_TRUE(speed_meter_bps(&m, 1000) == 0.0);
}

static void test_single_sample_is_zero_bps(void) {
    speed_meter_t m;
    speed_meter_init(&m);
    speed_meter_sample(&m, 1024, 100);
    // Less than two samples → cannot compute.
    TEST_ASSERT_TRUE(speed_meter_bps(&m, 200) == 0.0);
}

static void test_constant_throughput(void) {
    // 1 MB/s for 2 seconds = ~1048576 bps.
    speed_meter_t m;
    speed_meter_init(&m);
    for (int i = 0; i <= 10; i++) {
        // 200 ms apart, +204800 bytes each step → 204800/0.2 = 1024000 bps
        speed_meter_sample(&m, (uint64_t)(i * 204800), (uint64_t)(i * 200));
    }
    double bps = speed_meter_bps(&m, 2000);
    TEST_ASSERT_TRUE(bps > 1000000.0 && bps < 1100000.0);
}

static void test_window_drops_stale_samples(void) {
    speed_meter_t m;
    speed_meter_init(&m);
    // Old, fast burst at t = 0..200 ms.
    speed_meter_sample(&m, 0,        0);
    speed_meter_sample(&m, 10000000, 200);
    // Idle for 5 s, then sample again.
    speed_meter_sample(&m, 10000000, 5200);
    speed_meter_sample(&m, 10001000, 5400);
    // The 2-second window at now=5400 starts at 3400; the burst before
    // that should be ignored. Effective: 1000 bytes / 0.2 s = 5000 bps.
    double bps = speed_meter_bps(&m, 5400);
    TEST_ASSERT_TRUE(bps > 4000.0 && bps < 6000.0);
}

static void test_ring_buffer_overwrites(void) {
    speed_meter_t m;
    speed_meter_init(&m);
    // Push way more than SPEED_METER_SAMPLES samples.
    for (int i = 0; i < 200; i++) {
        speed_meter_sample(&m, (uint64_t)(i * 1024), (uint64_t)(i * 100));
    }
    // Last 16 samples cover ~1.6s, which fits in the 2s window.
    // Throughput: 1024 bytes / 100 ms = 10240 bps.
    double bps = speed_meter_bps(&m, 200 * 100 - 100);
    TEST_ASSERT_TRUE(bps > 8000.0 && bps < 12000.0);
}

static void test_null_safe(void) {
    speed_meter_init(NULL);          // must not crash
    speed_meter_sample(NULL, 1, 1);  // must not crash
    TEST_ASSERT_TRUE(speed_meter_bps(NULL, 100) == 0.0);
}

static void test_non_monotonic_returns_zero(void) {
    speed_meter_t m;
    speed_meter_init(&m);
    speed_meter_sample(&m, 1000, 100);
    speed_meter_sample(&m, 500,  200);  // counter went backwards
    TEST_ASSERT_TRUE(speed_meter_bps(&m, 200) == 0.0);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_zero_samples_is_zero_bps);
    RUN_TEST(test_single_sample_is_zero_bps);
    RUN_TEST(test_constant_throughput);
    RUN_TEST(test_window_drops_stale_samples);
    RUN_TEST(test_ring_buffer_overwrites);
    RUN_TEST(test_null_safe);
    RUN_TEST(test_non_monotonic_returns_zero);
    return UNITY_END();
}
