#ifndef GDRIVE_H
#define GDRIVE_H

#include <stddef.h>

// Convert a Google Drive sharing URL into a direct-download URL.
//
// Supported input formats:
//   - https://drive.google.com/file/d/<ID>/view
//   - https://drive.google.com/open?id=<ID>
//   - https://drive.google.com/uc?id=<ID>&export=download (idempotent)
//
// Folder URLs (https://drive.google.com/drive/folders/...) are NOT
// converted; the original URL is copied through unchanged.
//
// Non-Google-Drive URLs are copied through unchanged.
//
// `output_url` is always NUL-terminated as long as `output_size > 0`.
// Safe against NULL inputs (no-op).
void convert_gdrive_url(const char* input_url, char* output_url, size_t output_size);

#endif // GDRIVE_H
