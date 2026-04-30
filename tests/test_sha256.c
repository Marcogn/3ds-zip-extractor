// Unit tests for SHA-256 hashing + integrity verification.
#include "unity.h"
#include "../include/integrity.h"
#include <stdio.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static const char* SHA256_EMPTY =
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
static const char* SHA256_ABC =
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";

static void test_empty_string(void) {
    char hex[65];
    sha256_hex("", 0, hex);
    TEST_ASSERT_EQUAL_STRING(SHA256_EMPTY, hex);
}

static void test_abc(void) {
    char hex[65];
    sha256_hex("abc", 3, hex);
    TEST_ASSERT_EQUAL_STRING(SHA256_ABC, hex);
}

static void test_null_data_safe(void) {
    char hex[65];
    sha256_hex(NULL, 0, hex);
    TEST_ASSERT_EQUAL_STRING(SHA256_EMPTY, hex);
}

static void test_file_known(void) {
    // Hash the known fixture "fixtures/three_bytes.bin" which contains "abc".
    char hex[65];
    TEST_ASSERT_TRUE(sha256_hex_file("fixtures/three_bytes.bin", hex));
    TEST_ASSERT_EQUAL_STRING(SHA256_ABC, hex);
}

static void test_file_missing(void) {
    char hex[65];
    TEST_ASSERT_FALSE(sha256_hex_file("fixtures/does_not_exist", hex));
    TEST_ASSERT_EQUAL_STRING("", hex);
}

static void test_verify_integrity_match(void) {
    TEST_ASSERT_TRUE(verify_integrity("fixtures/three_bytes.bin", SHA256_ABC));
}

static void test_verify_integrity_case_insensitive(void) {
    char upper[65];
    for (int i = 0; i < 64; i++) {
        char c = SHA256_ABC[i];
        if (c >= 'a' && c <= 'f') c = (char)(c - 'a' + 'A');
        upper[i] = c;
    }
    upper[64] = '\0';
    TEST_ASSERT_TRUE(verify_integrity("fixtures/three_bytes.bin", upper));
}

static void test_verify_integrity_mismatch(void) {
    TEST_ASSERT_FALSE(verify_integrity("fixtures/three_bytes.bin", SHA256_EMPTY));
}

static void test_verify_integrity_null_safe(void) {
    TEST_ASSERT_FALSE(verify_integrity(NULL, SHA256_EMPTY));
    TEST_ASSERT_FALSE(verify_integrity("fixtures/three_bytes.bin", NULL));
    TEST_ASSERT_FALSE(verify_integrity("fixtures/three_bytes.bin", ""));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_empty_string);
    RUN_TEST(test_abc);
    RUN_TEST(test_null_data_safe);
    RUN_TEST(test_file_known);
    RUN_TEST(test_file_missing);
    RUN_TEST(test_verify_integrity_match);
    RUN_TEST(test_verify_integrity_case_insensitive);
    RUN_TEST(test_verify_integrity_mismatch);
    RUN_TEST(test_verify_integrity_null_safe);
    return UNITY_END();
}
