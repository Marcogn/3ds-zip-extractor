#ifndef ARCHIVE_EXTRACTOR_H
#define ARCHIVE_EXTRACTOR_H

#include <stdbool.h>

// Supported archive types
typedef enum {
    ARCHIVE_UNKNOWN = 0,
    ARCHIVE_ZIP,
    ARCHIVE_TAR,
    ARCHIVE_GZIP,      // Can be .gz or .tar.gz
    ARCHIVE_BZIP2,     // Can be .bz2 or .tar.bz2
    ARCHIVE_XZ,        // Can be .xz or .tar.xz
    ARCHIVE_7Z,
    ARCHIVE_RAR,
    ARCHIVE_ZSTD       // Can be .zst or .tar.zst
} ArchiveType;

// Callback for extraction progress
// Returns true to cancel extraction, false to continue
typedef bool (*ExtractCallback)(int file_count, const char* current_file, void* user_data);

// Detect archive type by file signature
ArchiveType detect_archive_type(const char* file_path);

// Get human-readable name for archive type
const char* get_archive_type_name(ArchiveType type);

// Extract archive using libarchive
// Returns number of files extracted, or negative value on error
int extract_archive_libarchive(const char* archive_path, const char* output_dir,
                                ExtractCallback callback, void* user_data);

// Check if file is a supported archive
bool is_supported_archive(const char* file_path);

// Count the number of entries in `path` by performing a read-only pass
// with libarchive. Returns the entry count, or -1 if the archive is not
// seekable, the format isn't recognised, or the API otherwise fails.
//
// Pre-counting allows the GUI to render a percentage progress bar during
// extraction. Callers may pass `-1` through to the extraction callback to
// fall back to the spinner when the count isn't available.
int archive_count_entries(const char* path);

#endif // ARCHIVE_EXTRACTOR_H
