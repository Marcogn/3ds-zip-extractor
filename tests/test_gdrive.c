// Unit tests for convert_gdrive_url.
#include "unity.h"
#include "../include/gdrive.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static void assert_contains(const char* haystack, const char* needle) {
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(haystack, needle), needle);
}

// /file/d/<ID>/view → uc?export=download&id=<ID>&confirm=t
static void test_file_d_view(void) {
    char out[512];
    convert_gdrive_url("https://drive.google.com/file/d/ABC123/view", out, sizeof(out));
    assert_contains(out, "drive.google.com/uc?export=download");
    assert_contains(out, "id=ABC123");
    assert_contains(out, "confirm=t");
}

static void test_open_id(void) {
    char out[512];
    convert_gdrive_url("https://drive.google.com/open?id=XYZ789", out, sizeof(out));
    assert_contains(out, "id=XYZ789");
    assert_contains(out, "confirm=t");
}

static void test_uc_id(void) {
    char out[512];
    convert_gdrive_url("https://drive.google.com/uc?id=DEF&export=download",
                       out, sizeof(out));
    assert_contains(out, "id=DEF");
}

static void test_non_gdrive_unchanged(void) {
    char out[512];
    convert_gdrive_url("https://example.com/foo.zip", out, sizeof(out));
    TEST_ASSERT_EQUAL_STRING("https://example.com/foo.zip", out);
}

static void test_folder_url_unchanged(void) {
    const char* in = "https://drive.google.com/drive/folders/abcd";
    char out[512];
    convert_gdrive_url(in, out, sizeof(out));
    TEST_ASSERT_EQUAL_STRING(in, out);
}

static void test_small_buffer_no_overflow(void) {
    // Buffer too small to hold the full converted URL — output must still be
    // NUL-terminated and not overrun. We sandwich the buffer between guards.
    char guard1[8];
    char out[16];
    char guard2[8];
    memset(guard1, 'A', sizeof(guard1));
    memset(guard2, 'B', sizeof(guard2));

    convert_gdrive_url("https://drive.google.com/file/d/ABCDEFGHIJ/view",
                       out, sizeof(out));
    // Guards must be untouched.
    for (size_t i = 0; i < sizeof(guard1); i++) TEST_ASSERT_EQUAL(guard1[i], 'A');
    for (size_t i = 0; i < sizeof(guard2); i++) TEST_ASSERT_EQUAL(guard2[i], 'B');
    // out must be NUL-terminated.
    TEST_ASSERT_EQUAL_CHAR('\0', out[sizeof(out) - 1]);
}

static void test_null_inputs_dont_crash(void) {
    char out[16];
    out[0] = 'X';
    convert_gdrive_url(NULL, out, sizeof(out));
    TEST_ASSERT_EQUAL_CHAR('\0', out[0]);

    // NULL out is also tolerated.
    convert_gdrive_url("https://example.com", NULL, 0);

    // Empty input.
    out[0] = 'X';
    convert_gdrive_url("", out, sizeof(out));
    TEST_ASSERT_EQUAL_STRING("", out);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_file_d_view);
    RUN_TEST(test_open_id);
    RUN_TEST(test_uc_id);
    RUN_TEST(test_non_gdrive_unchanged);
    RUN_TEST(test_folder_url_unchanged);
    RUN_TEST(test_small_buffer_no_overflow);
    RUN_TEST(test_null_inputs_dont_crash);
    return UNITY_END();
}
