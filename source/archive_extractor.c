#include "archive_extractor.h"
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

// Stub implementations for missing functions on 3DS
// umask is declared but not implemented in devkitARM
mode_t umask(mode_t mask) {
    return 0022;  // Return default umask
}

// getpwnam is declared but not implemented in devkitARM
struct passwd *getpwnam(const char *name) {
    return NULL;
}

// getgrnam is declared but not implemented in devkitARM
struct group *getgrnam(const char *name) {
    return NULL;
}

// LZ4 functions are not available in 3DS portlibs
int LZ4_decompress_safe(const char* src, char* dst, int compressedSize, int dstCapacity) {
    return -1;  // Not supported
}

int LZ4_decompress_safe_usingDict(const char* src, char* dst, int srcSize, int dstCapacity,
                                   const char* dictStart, int dictSize) {
    return -1;  // Not supported
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

// Detect archive type by file signature (magic bytes)
ArchiveType detect_archive_type(const char* file_path) {
    FILE* f = fopen(file_path, "rb");
    if (!f) {
        return ARCHIVE_UNKNOWN;
    }

    unsigned char sig[16] = {0};
    size_t read = fread(sig, 1, sizeof(sig), f);
    fclose(f);

    if (read < 4) {
        return ARCHIVE_UNKNOWN;
    }

    // ZIP: PK\x03\x04 or PK\x05\x06 (empty zip)
    if (sig[0] == 'P' && sig[1] == 'K' && (sig[2] == 0x03 || sig[2] == 0x05)) {
        return ARCHIVE_ZIP;
    }

    // GZIP: 0x1f 0x8b
    if (sig[0] == 0x1f && sig[1] == 0x8b) {
        return ARCHIVE_GZIP;
    }

    // BZIP2: BZ
    if (sig[0] == 'B' && sig[1] == 'Z') {
        return ARCHIVE_BZIP2;
    }

    // XZ: 0xFD 0x37 0x7A 0x58 0x5A 0x00
    if (sig[0] == 0xFD && sig[1] == 0x37 && sig[2] == 0x7A &&
        sig[3] == 0x58 && sig[4] == 0x5A && sig[5] == 0x00) {
        return ARCHIVE_XZ;
    }

    // 7z: 37 7A BC AF 27 1C
    if (sig[0] == 0x37 && sig[1] == 0x7A && sig[2] == 0xBC &&
        sig[3] == 0xAF && sig[4] == 0x27 && sig[5] == 0x1C) {
        return ARCHIVE_7Z;
    }

    // RAR: Rar!\x1a\x07 (RAR 4.x) or Rar!\x1a\x07\x01\x00 (RAR 5.x)
    if (sig[0] == 'R' && sig[1] == 'a' && sig[2] == 'r' && sig[3] == '!' &&
        sig[4] == 0x1a && sig[5] == 0x07) {
        return ARCHIVE_RAR;
    }

    // TAR (ustar format): "ustar" at offset 257
    // Need to read more data for TAR detection
    f = fopen(file_path, "rb");
    if (f) {
        char ustar[6] = {0};
        fseek(f, 257, SEEK_SET);
        if (fread(ustar, 1, 5, f) == 5) {
            if (strncmp(ustar, "ustar", 5) == 0) {
                fclose(f);
                return ARCHIVE_TAR;
            }
        }
        fclose(f);
    }

    // ZSTD: 0x28 0xB5 0x2F 0xFD
    if (sig[0] == 0x28 && sig[1] == 0xB5 && sig[2] == 0x2F && sig[3] == 0xFD) {
        return ARCHIVE_ZSTD;
    }

    return ARCHIVE_UNKNOWN;
}

// Get archive type name
const char* get_archive_type_name(ArchiveType type) {
    switch (type) {
        case ARCHIVE_ZIP: return "ZIP";
        case ARCHIVE_TAR: return "TAR";
        case ARCHIVE_GZIP: return "GZIP/TAR.GZ";
        case ARCHIVE_BZIP2: return "BZIP2/TAR.BZ2";
        case ARCHIVE_XZ: return "XZ/TAR.XZ";
        case ARCHIVE_7Z: return "7-Zip";
        case ARCHIVE_RAR: return "RAR";
        case ARCHIVE_ZSTD: return "ZSTD/TAR.ZSTD";
        default: return "Unknown";
    }
}

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

    // Use minimal options to avoid POSIX functions not available on 3DS
    archive_write_disk_set_options(ext,
        ARCHIVE_EXTRACT_TIME |
        ARCHIVE_EXTRACT_SECURE_NODOTDOT);

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

// Check if file is a supported archive
bool is_supported_archive(const char* file_path) {
    ArchiveType type = detect_archive_type(file_path);
    return type != ARCHIVE_UNKNOWN;
}
