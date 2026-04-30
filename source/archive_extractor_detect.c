// Pure archive detection (magic-byte sniffing). No libarchive, no 3ds.h
// dependency — host-testable.

#include "archive_extractor.h"

#include <stdio.h>
#include <string.h>

ArchiveType detect_archive_type(const char* file_path) {
    if (file_path == NULL) {
        return ARCHIVE_UNKNOWN;
    }

    FILE* f = fopen(file_path, "rb");
    if (!f) {
        return ARCHIVE_UNKNOWN;
    }

    unsigned char sig[16] = {0};
    size_t got = fread(sig, 1, sizeof(sig), f);

    if (got < 4) {
        fclose(f);
        return ARCHIVE_UNKNOWN;
    }

    // ZIP: PK\x03\x04 or PK\x05\x06 (empty zip).
    if (sig[0] == 'P' && sig[1] == 'K' && (sig[2] == 0x03 || sig[2] == 0x05)) {
        fclose(f);
        return ARCHIVE_ZIP;
    }
    // GZIP.
    if (sig[0] == 0x1f && sig[1] == 0x8b) {
        fclose(f);
        return ARCHIVE_GZIP;
    }
    // BZIP2.
    if (sig[0] == 'B' && sig[1] == 'Z') {
        fclose(f);
        return ARCHIVE_BZIP2;
    }
    // XZ.
    if (got >= 6 &&
        sig[0] == 0xFD && sig[1] == 0x37 && sig[2] == 0x7A &&
        sig[3] == 0x58 && sig[4] == 0x5A && sig[5] == 0x00) {
        fclose(f);
        return ARCHIVE_XZ;
    }
    // 7z.
    if (got >= 6 &&
        sig[0] == 0x37 && sig[1] == 0x7A && sig[2] == 0xBC &&
        sig[3] == 0xAF && sig[4] == 0x27 && sig[5] == 0x1C) {
        fclose(f);
        return ARCHIVE_7Z;
    }
    // RAR (4.x and 5.x both share the first 6 bytes).
    if (got >= 6 &&
        sig[0] == 'R' && sig[1] == 'a' && sig[2] == 'r' && sig[3] == '!' &&
        sig[4] == 0x1a && sig[5] == 0x07) {
        fclose(f);
        return ARCHIVE_RAR;
    }
    // ZSTD.
    if (sig[0] == 0x28 && sig[1] == 0xB5 && sig[2] == 0x2F && sig[3] == 0xFD) {
        fclose(f);
        return ARCHIVE_ZSTD;
    }

    // TAR (ustar magic at offset 257). Reuse the open file handle.
    char ustar[6] = {0};
    if (fseek(f, 257, SEEK_SET) == 0 &&
        fread(ustar, 1, 5, f) == 5 &&
        strncmp(ustar, "ustar", 5) == 0) {
        fclose(f);
        return ARCHIVE_TAR;
    }
    fclose(f);

    return ARCHIVE_UNKNOWN;
}

const char* get_archive_type_name(ArchiveType type) {
    switch (type) {
        case ARCHIVE_ZIP:   return "ZIP";
        case ARCHIVE_TAR:   return "TAR";
        case ARCHIVE_GZIP:  return "GZIP/TAR.GZ";
        case ARCHIVE_BZIP2: return "BZIP2/TAR.BZ2";
        case ARCHIVE_XZ:    return "XZ/TAR.XZ";
        case ARCHIVE_7Z:    return "7-Zip";
        case ARCHIVE_RAR:   return "RAR";
        case ARCHIVE_ZSTD:  return "ZSTD/TAR.ZSTD";
        default:            return "Unknown";
    }
}

bool is_supported_archive(const char* file_path) {
    return detect_archive_type(file_path) != ARCHIVE_UNKNOWN;
}
