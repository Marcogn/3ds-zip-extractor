// Unit tests for url_input pure helpers.
#include "unity.h"
#include "../include/url_input.h"
#include "../include/config.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void setUp(void) {}
void tearDown(void) {}

static void test_null_is_invalid(void) {
    TEST_ASSERT_FALSE(url_is_valid_http(NULL));
}

static void test_empty_is_invalid(void) {
    TEST_ASSERT_FALSE(url_is_valid_http(""));
}

static void test_http_minimal_valid(void) {
    TEST_ASSERT_TRUE(url_is_valid_http("http://x"));
}

static void test_https_minimal_valid(void) {
    TEST_ASSERT_TRUE(url_is_valid_http("https://x"));
}

static void test_https_just_scheme_invalid(void) {
    TEST_ASSERT_FALSE(url_is_valid_http("https://"));
    TEST_ASSERT_FALSE(url_is_valid_http("http://"));
}

static void test_ftp_invalid(void) {
    TEST_ASSERT_FALSE(url_is_valid_http("ftp://example.com/file.zip"));
}

static void test_relative_invalid(void) {
    TEST_ASSERT_FALSE(url_is_valid_http("example.com/file.zip"));
    TEST_ASSERT_FALSE(url_is_valid_http("/path"));
    TEST_ASSERT_FALSE(url_is_valid_http("file:///etc/passwd"));
}

static void test_uppercase_scheme_invalid(void) {
    // We require lowercase scheme as users do; be conservative.
    TEST_ASSERT_FALSE(url_is_valid_http("HTTP://example.com"));
    TEST_ASSERT_FALSE(url_is_valid_http("Https://example.com"));
}

static void test_too_long_invalid(void) {
    char buf[MAX_URL_LENGTH + 16];
    memset(buf, 'a', sizeof(buf));
    memcpy(buf, "https://", 8);
    buf[sizeof(buf) - 1] = '\0';
    TEST_ASSERT_FALSE(url_is_valid_http(buf));
}

static void test_almost_too_long_valid(void) {
    char buf[MAX_URL_LENGTH];
    memset(buf, 'a', sizeof(buf));
    memcpy(buf, "https://", 8);
    buf[sizeof(buf) - 1] = '\0';  // length = MAX_URL_LENGTH - 1
    TEST_ASSERT_TRUE(url_is_valid_http(buf));
}

static void test_typical_valid(void) {
    TEST_ASSERT_TRUE(url_is_valid_http("https://example.com/big.zip"));
    TEST_ASSERT_TRUE(url_is_valid_http(
        "https://drive.google.com/file/d/abc123/view"));
}

static void test_append_to_config_creates_new_lines(void) {
    const char* path = "fixtures/url_append_test.txt";
    remove(path);
    // Seed the file with a baseline content.
    FILE* f = fopen(path, "wb");
    TEST_ASSERT_NOT_NULL(f);
    fprintf(f, "extract_path=sdmc:/extracted/\n");
    fprintf(f, "https://existing.example.com/a.zip\n");
    fclose(f);

    TEST_ASSERT_TRUE(url_input_append_to_config(path, "https://added.example.com/b.zip"));

    f = fopen(path, "rb");
    TEST_ASSERT_NOT_NULL(f);
    char buf[1024]; size_t n = fread(buf, 1, sizeof(buf) - 1, f); buf[n] = '\0';
    fclose(f);
    // Original lines preserved verbatim at the start.
    TEST_ASSERT_EQUAL(0, strncmp(buf,
        "extract_path=sdmc:/extracted/\nhttps://existing.example.com/a.zip\n",
        65));
    // Added comment + URL appear afterwards.
    TEST_ASSERT_NOT_NULL(strstr(buf, "# added in-app "));
    TEST_ASSERT_NOT_NULL(strstr(buf, "https://added.example.com/b.zip"));

    remove(path);
}

static void test_append_to_config_rejects_invalid(void) {
    const char* path = "fixtures/url_append_invalid.txt";
    remove(path);
    FILE* f = fopen(path, "wb");
    TEST_ASSERT_NOT_NULL(f);
    fprintf(f, "https://x\n");
    fclose(f);

    TEST_ASSERT_FALSE(url_input_append_to_config(path, "ftp://nope"));
    TEST_ASSERT_FALSE(url_input_append_to_config(path, NULL));
    TEST_ASSERT_FALSE(url_input_append_to_config(NULL, "https://ok.example.com"));

    // File untouched.
    f = fopen(path, "rb");
    char buf[64]; size_t n = fread(buf, 1, sizeof(buf) - 1, f); buf[n] = '\0';
    fclose(f);
    TEST_ASSERT_EQUAL_STRING("https://x\n", buf);

    remove(path);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_null_is_invalid);
    RUN_TEST(test_empty_is_invalid);
    RUN_TEST(test_http_minimal_valid);
    RUN_TEST(test_https_minimal_valid);
    RUN_TEST(test_https_just_scheme_invalid);
    RUN_TEST(test_ftp_invalid);
    RUN_TEST(test_relative_invalid);
    RUN_TEST(test_uppercase_scheme_invalid);
    RUN_TEST(test_too_long_invalid);
    RUN_TEST(test_almost_too_long_valid);
    RUN_TEST(test_typical_valid);
    RUN_TEST(test_append_to_config_creates_new_lines);
    RUN_TEST(test_append_to_config_rejects_invalid);
    return UNITY_END();
}
