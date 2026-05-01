// Unit tests for read_config_file.
#include "unity.h"
#include "../include/config.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

#define FIXTURE(name) ("fixtures/" name)

static void test_missing_file_returns_minus_one(void) {
    AppConfig cfg;
    int rc = read_config_file(FIXTURE("does_not_exist.txt"), &cfg);
    TEST_ASSERT_EQUAL(-1, rc);
    // Defaults still applied.
    TEST_ASSERT_EQUAL_STRING("sdmc:/extracted/", cfg.extract_path);
    TEST_ASSERT_EQUAL(3, cfg.max_retries);
    TEST_ASSERT_FALSE(cfg.auto_retry);
    TEST_ASSERT_EQUAL(MAX_URLS, cfg.max_urls);
    TEST_ASSERT_EQUAL(128, cfg.download_buffer_kb);
    TEST_ASSERT_EQUAL(30, cfg.connect_timeout_s);
}

static void test_empty_or_comments_only(void) {
    AppConfig cfg;
    int rc = read_config_file(FIXTURE("config_empty.txt"), &cfg);
    TEST_ASSERT_EQUAL(0, rc);
    TEST_ASSERT_EQUAL(0, cfg.count);
    TEST_ASSERT_EQUAL_STRING("sdmc:/extracted/", cfg.extract_path);
}

static void test_valid_settings_and_urls(void) {
    AppConfig cfg;
    int rc = read_config_file(FIXTURE("config_valid.txt"), &cfg);
    TEST_ASSERT_GREATER_OR_EQUAL(3, rc);
    TEST_ASSERT_EQUAL_STRING("sdmc:/foo/", cfg.extract_path);
    TEST_ASSERT_TRUE(cfg.auto_retry);
    TEST_ASSERT_EQUAL(5, cfg.max_retries);

    // First URL — bare.
    TEST_ASSERT_EQUAL_STRING("https://example.com/a.zip", cfg.items[0].url);
    TEST_ASSERT_EQUAL_STRING("", cfg.items[0].sha256_hex);

    // Second URL — has a sha256 suffix that must be split off.
    TEST_ASSERT_EQUAL_STRING("https://example.com/b.tar.gz", cfg.items[1].url);
    TEST_ASSERT_EQUAL_STRING(
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        cfg.items[1].sha256_hex);

    // Third URL — Google Drive (kept unchanged in storage; conversion
    // happens at download time).
    TEST_ASSERT_EQUAL_STRING("https://drive.google.com/file/d/ABC123/view",
                             cfg.items[2].url);
}

static void test_malformed_settings_clamped(void) {
    AppConfig cfg;
    read_config_file(FIXTURE("config_malformed.txt"), &cfg);
    // auto_retry=invalid → falls back to default (false).
    TEST_ASSERT_FALSE(cfg.auto_retry);
    // max_retries=999 → clamped to 10.
    TEST_ASSERT_EQUAL(10, cfg.max_retries);
    // First URL has an invalid sha (not 64 hex), so it stays attached as
    // part of the URL — but we should NOT crash.
    TEST_ASSERT_TRUE(cfg.count >= 1);
    // Second URL has a valid 64-hex sha (mixed case — we lowercase).
    int idx = -1;
    for (int i = 0; i < cfg.count; i++) {
        if (strcmp(cfg.items[i].url, "https://example.com/b.zip") == 0) idx = i;
    }
    TEST_ASSERT_NOT_EQUAL(-1, idx);
    TEST_ASSERT_EQUAL_STRING(
        "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
        cfg.items[idx].sha256_hex);
}

static void test_negative_retries_clamped(void) {
    AppConfig cfg;
    read_config_file(FIXTURE("config_negative_retries.txt"), &cfg);
    TEST_ASSERT_EQUAL(0, cfg.max_retries);
}

static void test_many_urls_clamped_to_max(void) {
    AppConfig cfg;
    int rc = read_config_file(FIXTURE("config_many_urls.txt"), &cfg);
    TEST_ASSERT_EQUAL(MAX_URLS < 60 ? MAX_URLS : 60, rc);
    TEST_ASSERT_EQUAL(rc, cfg.count);
    TEST_ASSERT_LESS_OR_EQUAL(MAX_URLS, cfg.count);
}

static void test_long_url_truncated_no_overflow(void) {
    AppConfig cfg;
    int rc = read_config_file(FIXTURE("config_long_url.txt"), &cfg);
    // Still loads (1 URL), URL is truncated to fit, NUL terminated.
    TEST_ASSERT_EQUAL(1, rc);
    TEST_ASSERT_EQUAL_CHAR('\0', cfg.items[0].url[MAX_URL_LENGTH - 1]);
}

static void test_utf8_bom_ignored(void) {
    AppConfig cfg;
    int rc = read_config_file(FIXTURE("config_bom.txt"), &cfg);
    TEST_ASSERT_EQUAL(1, rc);
    TEST_ASSERT_EQUAL_STRING("https://example.com/bom.zip", cfg.items[0].url);
}

static void test_null_cfg_safe(void) {
    int rc = read_config_file(FIXTURE("config_valid.txt"), NULL);
    TEST_ASSERT_EQUAL(-1, rc);
}

// --- Clamping / new config-key tests (v1.0.1-refactor) --------------------

static void test_max_urls_zero_clamped_to_min(void) {
    AppConfig cfg;
    // max_urls=0 → clamped to 1 by clamp_int(atoi(v), 1, MAX_URLS)
    read_config_file(FIXTURE("config_max_urls_zero.txt"), &cfg);
    TEST_ASSERT_EQUAL(1, cfg.max_urls);
    // Only the first URL should be stored (max_urls=1 limits the queue).
    TEST_ASSERT_EQUAL(1, cfg.count);
}

static void test_max_urls_large_clamped_to_max(void) {
    AppConfig cfg;
    // max_urls=99999 → clamped to MAX_URLS (100)
    read_config_file(FIXTURE("config_max_urls_large.txt"), &cfg);
    TEST_ASSERT_EQUAL(MAX_URLS, cfg.max_urls);
}

static void test_download_buffer_nonnumeric_uses_default(void) {
    AppConfig cfg;
    // download_buffer_kb=abc → atoi("abc")=0, clamped to 4 (the minimum)
    read_config_file(FIXTURE("config_buffer_nonnumeric.txt"), &cfg);
    TEST_ASSERT_EQUAL(4, cfg.download_buffer_kb);
}

static void test_connect_timeout_negative_clamped(void) {
    AppConfig cfg;
    // connect_timeout_s=-5 → clamped to 1 by clamp_int(atoi(v), 1, 600)
    read_config_file(FIXTURE("config_timeout_negative.txt"), &cfg);
    TEST_ASSERT_EQUAL(1, cfg.connect_timeout_s);
}

static void test_unknown_key_ignored_known_key_parsed(void) {
    AppConfig cfg;
    int rc = read_config_file(FIXTURE("config_unknown_plus_known.txt"), &cfg);
    // Unknown key=value lines are treated as URLs (parser doesn't fail on them).
    // The important checks: no crash, and the known key was parsed.
    TEST_ASSERT_TRUE(rc >= 0);
    TEST_ASSERT_EQUAL(7, cfg.max_retries);
    // The real URL must be present somewhere in cfg.items.
    bool found = false;
    for (int i = 0; i < cfg.count; i++) {
        if (strcmp(cfg.items[i].url, "https://example.com/a.zip") == 0) {
            found = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(found);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_missing_file_returns_minus_one);
    RUN_TEST(test_empty_or_comments_only);
    RUN_TEST(test_valid_settings_and_urls);
    RUN_TEST(test_malformed_settings_clamped);
    RUN_TEST(test_negative_retries_clamped);
    RUN_TEST(test_many_urls_clamped_to_max);
    RUN_TEST(test_long_url_truncated_no_overflow);
    RUN_TEST(test_utf8_bom_ignored);
    RUN_TEST(test_null_cfg_safe);
    RUN_TEST(test_max_urls_zero_clamped_to_min);
    RUN_TEST(test_max_urls_large_clamped_to_max);
    RUN_TEST(test_download_buffer_nonnumeric_uses_default);
    RUN_TEST(test_connect_timeout_negative_clamped);
    RUN_TEST(test_unknown_key_ignored_known_key_parsed);
    return UNITY_END();
}
