#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <3ds.h>
#include <stdbool.h>

#include "config.h"

// Download `url` to `output_path`, with optional resume and progress callback.
//
// `*cancelled` is set to true if the user pressed B during download.
// `current` and `total` are the queue position passed through to the GUI.
// Buffer size and connect timeout come from `cfg`.
//
// Returns 0 on success, -1 on hard failure, -2 on user cancel.
Result download_file(const AppConfig* cfg,
                     const char* url, const char* output_path,
                     bool* cancelled, int current, int total);

// Build the per-run unique temporary download path under
// `sdmc:/3ds/zip-extractor/tmp/`. The directory is created if missing.
// `out` will receive a NUL-terminated path; `out_size` must be >= 96.
void download_make_temp_path(char* out, size_t out_size);

// Remove orphaned `.part` files from previous runs in the temp directory.
void download_cleanup_orphans(void);

#endif // DOWNLOAD_H
