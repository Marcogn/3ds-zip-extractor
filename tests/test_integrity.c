// Unit tests for verify_integrity() — host build (no 3DS, no sdmc: paths).
//
// Temporary files are created with mkstemp() in /tmp so that the path
// can be passed to verify_integrity() which requires a char* path.
// Each test creates its own temp file and unlinks it when done.
#define _POSIX_C_SOURCE 200809L
#include "unity.h"
#include "../include/integrity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // mkstemp, unlink, write, close

void setUp(void) {}
void tearDown(void) {}

// SHA-256 of the empty string.
static const char* SHA256_EMPTY =
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

// Helper: write `content`/`len` to a fresh temp file, return the path in
// `out_path` (must be at least 32 bytes).  Returns the fd (caller must
// close it) or -1 on error.
static int make_tmpfile(const char* content, size_t len, char* out_path) {
    strcpy(out_path, "/tmp/test_integrity_XXXXXX");
    int fd = mkstemp(out_path);
    if (fd == -1) return -1;
    if (len > 0) {
        ssize_t written = write(fd, content, len);
        if (written < 0 || (size_t)written != len) {
            close(fd);
            unlink(out_path);
            return -1;
        }
    }
    close(fd);
    return fd;  // already closed, but non-negative signals success
}

// 1. Known content → correct SHA-256 → verify_integrity returns true.
static void test_known_content_correct_hash_passes(void) {
    char path[64];
    int rc = make_tmpfile("hello world", 11, path);
    TEST_ASSERT_NOT_EQUAL(-1, rc);

    // Compute the expected hash from the file itself.
    char expected[65];
    TEST_ASSERT_TRUE(sha256_hex_file(path, expected));

    TEST_ASSERT_TRUE(verify_integrity(path, expected));
    unlink(path);
}

// 2. Known content → wrong SHA-256 → returns false.
static void test_known_content_wrong_hash_fails(void) {
    char path[64];
    int rc = make_tmpfile("hello world", 11, path);
    TEST_ASSERT_NOT_EQUAL(-1, rc);

    // SHA256_EMPTY is the hash of the empty string, not "hello world".
    TEST_ASSERT_FALSE(verify_integrity(path, SHA256_EMPTY));
    unlink(path);
}

// 3. Path that does not exist → returns false.
static void test_nonexistent_path_fails(void) {
    TEST_ASSERT_FALSE(
        verify_integrity("/tmp/test_integrity_does_not_exist_xyz.bin",
                         SHA256_EMPTY));
}

// 4. Hash shorter than 64 hex chars → returns false.
static void test_hash_too_short_fails(void) {
    char path[64];
    int rc = make_tmpfile("data", 4, path);
    TEST_ASSERT_NOT_EQUAL(-1, rc);

    // 32 hex chars — half the required length.
    TEST_ASSERT_FALSE(verify_integrity(path, "e3b0c44298fc1c149afbf4c8996fb924"));
    unlink(path);
}

// 5. Hash longer than 64 hex chars → returns false.
static void test_hash_too_long_fails(void) {
    char path[64];
    int rc = make_tmpfile("data", 4, path);
    TEST_ASSERT_NOT_EQUAL(-1, rc);

    // 65 hex chars (one extra digit appended).
    TEST_ASSERT_FALSE(
        verify_integrity(path,
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b8550"));
    unlink(path);
}

// 6. Hash string containing non-hex characters → returns false.
static void test_hash_nonhex_chars_fails(void) {
    char path[64];
    int rc = make_tmpfile("data", 4, path);
    TEST_ASSERT_NOT_EQUAL(-1, rc);

    // Replace the last 8 chars of a valid-length string with 'Z' (non-hex).
    TEST_ASSERT_FALSE(
        verify_integrity(path,
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991bZZZZZZZZ"));
    unlink(path);
}

// 7. Empty file → SHA-256 of the empty string → returns true.
static void test_empty_file_passes(void) {
    char path[64];
    int rc = make_tmpfile(NULL, 0, path);
    TEST_ASSERT_NOT_EQUAL(-1, rc);

    TEST_ASSERT_TRUE(verify_integrity(path, SHA256_EMPTY));
    unlink(path);
}

// 8. Uppercase hex digits in the expected hash → accepted (case-insensitive).
//    verify_integrity() normalises via tolower() before comparing.
static void test_uppercase_hash_accepted(void) {
    char path[64];
    int rc = make_tmpfile("abc", 3, path);
    TEST_ASSERT_NOT_EQUAL(-1, rc);

    // Get the lowercase hash first.
    char lower_hex[65];
    TEST_ASSERT_TRUE(sha256_hex_file(path, lower_hex));

    // Convert to uppercase.
    char upper_hex[65];
    for (int i = 0; i < 64; i++) {
        char c = lower_hex[i];
        if (c >= 'a' && c <= 'f') c = (char)(c - 'a' + 'A');
        upper_hex[i] = c;
    }
    upper_hex[64] = '\0';

    // verify_integrity must treat upper and lower as equivalent.
    TEST_ASSERT_TRUE(verify_integrity(path, upper_hex));
    unlink(path);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_known_content_correct_hash_passes);
    RUN_TEST(test_known_content_wrong_hash_fails);
    RUN_TEST(test_nonexistent_path_fails);
    RUN_TEST(test_hash_too_short_fails);
    RUN_TEST(test_hash_too_long_fails);
    RUN_TEST(test_hash_nonhex_chars_fails);
    RUN_TEST(test_empty_file_passes);
    RUN_TEST(test_uppercase_hash_accepted);
    return UNITY_END();
}
