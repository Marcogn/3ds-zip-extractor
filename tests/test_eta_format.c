// Unit tests for ETA formatter.
#include "unity.h"
#include "../include/eta_format.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static void test_negative_is_unknown(void) {
    char buf[8];
    format_eta(-1, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("--:--", buf);
    format_eta(-9999, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("--:--", buf);
}

static void test_zero(void) {
    char buf[8];
    format_eta(0, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("00:00", buf);
}

static void test_seconds_only(void) {
    char buf[8];
    format_eta(7, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("00:07", buf);
    format_eta(59, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("00:59", buf);
}

static void test_minute_boundary(void) {
    char buf[8];
    format_eta(60, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("01:00", buf);
    format_eta(125, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("02:05", buf);
}

static void test_cap_at_99_59(void) {
    char buf[8];
    format_eta(99 * 60 + 59, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("99:59", buf);
    format_eta(99 * 60 + 60, buf, sizeof(buf));  // overflow
    TEST_ASSERT_EQUAL_STRING("99:59", buf);
    format_eta(99999999, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("99:59", buf);
}

static void test_short_buffer_is_safe(void) {
    char buf[3] = "XX";
    format_eta(60, buf, 3);
    // Too small → leaves an empty string.
    TEST_ASSERT_EQUAL_STRING("", buf);
}

static void test_null_buffer_is_safe(void) {
    format_eta(60, NULL, 0);  // must not crash
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_negative_is_unknown);
    RUN_TEST(test_zero);
    RUN_TEST(test_seconds_only);
    RUN_TEST(test_minute_boundary);
    RUN_TEST(test_cap_at_99_59);
    RUN_TEST(test_short_buffer_is_safe);
    RUN_TEST(test_null_buffer_is_safe);
    return UNITY_END();
}
