// Unit tests for detect_archive_type / get_archive_type_name.
#include "unity.h"
#include "../include/archive_extractor.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

#define FIXTURE(name) ("fixtures/" name)

static void test_zip(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_ZIP, detect_archive_type(FIXTURE("tiny.zip")));
}
static void test_gzip(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_GZIP, detect_archive_type(FIXTURE("tiny.gz")));
}
static void test_bzip2(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_BZIP2, detect_archive_type(FIXTURE("tiny.bz2")));
}
static void test_xz(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_XZ, detect_archive_type(FIXTURE("tiny.xz")));
}
static void test_zstd(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_ZSTD, detect_archive_type(FIXTURE("tiny.zst")));
}
static void test_7z(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_7Z, detect_archive_type(FIXTURE("tiny.7z")));
}
static void test_rar(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_RAR, detect_archive_type(FIXTURE("tiny.rar")));
}
static void test_tar(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_TAR, detect_archive_type(FIXTURE("tiny.tar")));
}

static void test_too_short(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_UNKNOWN, detect_archive_type(FIXTURE("three_bytes.bin")));
    TEST_ASSERT_EQUAL(ARCHIVE_UNKNOWN, detect_archive_type(FIXTURE("empty.bin")));
}

static void test_not_an_archive(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_UNKNOWN, detect_archive_type(FIXTURE("not_an_archive.bin")));
}

static void test_missing_file(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_UNKNOWN, detect_archive_type(FIXTURE("does_not_exist.bin")));
}

static void test_null_safe(void) {
    TEST_ASSERT_EQUAL(ARCHIVE_UNKNOWN, detect_archive_type(NULL));
}

static void test_type_names(void) {
    TEST_ASSERT_EQUAL_STRING("ZIP",          get_archive_type_name(ARCHIVE_ZIP));
    TEST_ASSERT_EQUAL_STRING("TAR",          get_archive_type_name(ARCHIVE_TAR));
    TEST_ASSERT_EQUAL_STRING("GZIP/TAR.GZ",  get_archive_type_name(ARCHIVE_GZIP));
    TEST_ASSERT_EQUAL_STRING("BZIP2/TAR.BZ2",get_archive_type_name(ARCHIVE_BZIP2));
    TEST_ASSERT_EQUAL_STRING("XZ/TAR.XZ",    get_archive_type_name(ARCHIVE_XZ));
    TEST_ASSERT_EQUAL_STRING("7-Zip",        get_archive_type_name(ARCHIVE_7Z));
    TEST_ASSERT_EQUAL_STRING("RAR",          get_archive_type_name(ARCHIVE_RAR));
    TEST_ASSERT_EQUAL_STRING("ZSTD/TAR.ZSTD",get_archive_type_name(ARCHIVE_ZSTD));
    TEST_ASSERT_EQUAL_STRING("Unknown",      get_archive_type_name(ARCHIVE_UNKNOWN));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_zip);
    RUN_TEST(test_gzip);
    RUN_TEST(test_bzip2);
    RUN_TEST(test_xz);
    RUN_TEST(test_zstd);
    RUN_TEST(test_7z);
    RUN_TEST(test_rar);
    RUN_TEST(test_tar);
    RUN_TEST(test_too_short);
    RUN_TEST(test_not_an_archive);
    RUN_TEST(test_missing_file);
    RUN_TEST(test_null_safe);
    RUN_TEST(test_type_names);
    return UNITY_END();
}
