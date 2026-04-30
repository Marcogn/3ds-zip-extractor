#include "archive_extractor.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <archive.h>
#include <archive_entry.h>
#include <3ds.h>

// Stub implementations for symbols missing on devkitARM. Each one logs a
// one-shot warning to stderr the first time it's called so that unexpected
// usage surfaces during testing instead of failing silently.
//
// These stubs are intentionally honest: they don't simulate the real
// behaviour, they just keep the link step happy and tell us when libarchive
// (or any other dependency) actually reaches for them.

static void warn_once(bool* warned, const char* name) {
    if (!*warned) {
        *warned = true;
        fprintf(stderr,
                "[3ds-zip-extractor] stub: %s called (no-op on 3DS)\n",
                name);
    }
}

mode_t umask(mode_t mask) {
    static bool warned = false;
    warn_once(&warned, "umask");
    (void)mask;
    return 0022;  // Return default umask.
}

struct passwd *getpwnam(const char *name) {
    static bool warned = false;
    warn_once(&warned, "getpwnam");
    (void)name;
    return NULL;
}

struct group *getgrnam(const char *name) {
    static bool warned = false;
    warn_once(&warned, "getgrnam");
    (void)name;
    return NULL;
}

// LZ4 is not available in 3DS portlibs. The stubs return error so libarchive
// will refuse LZ4-compressed archives with a clear status code.
int LZ4_decompress_safe(const char* src, char* dst, int compressedSize, int dstCapacity) {
    static bool warned = false;
    warn_once(&warned, "LZ4_decompress_safe");
    (void)src; (void)dst; (void)compressedSize; (void)dstCapacity;
    return -1;
}

int LZ4_decompress_safe_usingDict(const char* src, char* dst, int srcSize, int dstCapacity,
                                   const char* dictStart, int dictSize) {
    static bool warned = false;
    warn_once(&warned, "LZ4_decompress_safe_usingDict");
    (void)src; (void)dst; (void)srcSize; (void)dstCapacity;
    (void)dictStart; (void)dictSize;
    return -1;
}

#define BUFFER_SIZE (128 * 1024)  // 128KB buffer for extraction

// Create directory recursively
static int mkdir_recursive(const char* path) {
    char tmp[512];
    char* p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0777);
            *p = '/';
        }
    }
    return mkdir(tmp, 0777);
}

// Detect / type-name / is_supported_archive moved to archive_extractor_detect.c
// (kept pure to allow host-side unit tests).

// Extract archive using libarchive (supports multiple formats)
int extract_archive_libarchive(const char* archive_path, const char* output_dir,
                                ExtractCallback callback, void* user_data) {
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int r;
    int file_count = 0;
    bool cancelled = false;

    // Create output directory
    mkdir_recursive(output_dir);

    // Create archive reader
    a = archive_read_new();
    if (!a) {
        return -1;
    }

    // Enable all supported formats and filters except LZ4 (not available on 3DS)
    archive_read_support_format_all(a);
    archive_read_support_filter_bzip2(a);
    archive_read_support_filter_gzip(a);
    archive_read_support_filter_xz(a);
    archive_read_support_filter_zstd(a);

    // Open the archive
    r = archive_read_open_filename(a, archive_path, BUFFER_SIZE);
    if (r != ARCHIVE_OK) {
        archive_read_free(a);
        return -2;
    }

    // Create archive writer for disk
    ext = archive_write_disk_new();
    if (!ext) {
        archive_read_free(a);
        return -3;
    }

    // Hardening: prevent zip-slip, symlink attacks, and absolute-path writes.
    // ARCHIVE_EXTRACT_TIME is preserved so timestamps are restored.
    archive_write_disk_set_options(ext,
        ARCHIVE_EXTRACT_TIME |
        ARCHIVE_EXTRACT_SECURE_NODOTDOT |
        ARCHIVE_EXTRACT_SECURE_SYMLINKS |
        ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS);

    // Don't use standard lookup - causes issues with getpwnam/getgrnam on 3DS
    // archive_write_disk_set_standard_lookup(ext);

    // Extract files
    while (true) {
        r = archive_read_next_header(a, &entry);

        if (r == ARCHIVE_EOF) {
            break;
        }

        if (r != ARCHIVE_OK) {
            // Error reading header, but continue
            continue;
        }

        // Get entry path
        const char* entry_path = archive_entry_pathname(entry);
        if (!entry_path) {
            continue;
        }

        file_count++;

        // Build full output path
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s%s", output_dir, entry_path);
        archive_entry_set_pathname(entry, fullpath);

        // Callback for progress update
        if (callback) {
            cancelled = callback(file_count, entry_path, user_data);
            if (cancelled) {
                break;
            }
        }

        // Write header
        r = archive_write_header(ext, entry);
        if (r == ARCHIVE_OK) {
            // Copy data
            if (archive_entry_size(entry) > 0) {
                const void *buff;
                size_t size;
                la_int64_t offset;

                while (true) {
                    r = archive_read_data_block(a, &buff, &size, &offset);
                    if (r == ARCHIVE_EOF) {
                        break;
                    }
                    if (r != ARCHIVE_OK) {
                        break;
                    }

                    r = archive_write_data_block(ext, buff, size, offset);
                    if (r != ARCHIVE_OK) {
                        break;
                    }
                }
            }

            r = archive_write_finish_entry(ext);
        }
    }

    // Cleanup
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);

    return cancelled ? -4 : file_count;
}

int archive_count_entries(const char* path) {
    if (!path) return -1;

    struct archive* a = archive_read_new();
    if (!a) return -1;

    archive_read_support_format_all(a);
    archive_read_support_filter_bzip2(a);
    archive_read_support_filter_gzip(a);
    archive_read_support_filter_xz(a);
    archive_read_support_filter_zstd(a);

    int r = archive_read_open_filename(a, path, BUFFER_SIZE);
    if (r != ARCHIVE_OK) {
        archive_read_free(a);
        return -1;
    }

    int count = 0;
    struct archive_entry* entry;
    while (true) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF) break;
        if (r == ARCHIVE_FATAL) {
            archive_read_close(a);
            archive_read_free(a);
            return -1;
        }
        if (r != ARCHIVE_OK && r != ARCHIVE_WARN) continue;
        count++;
    }

    archive_read_close(a);
    archive_read_free(a);
    return count;
}
