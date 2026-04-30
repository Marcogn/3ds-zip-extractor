#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>

// Hard limits used to size the in-memory configuration.
// `max_urls` in `AppConfig` may further reduce the number of URLs accepted
// at runtime (clamped to MAX_URLS).
#define MAX_URLS         100
#define MAX_URL_LENGTH   512
#define MAX_PATH_LENGTH  256
#define SHA256_HEX_LEN   64   // 32 bytes hex-encoded

// Per-URL download state (used by the queue/UI; lives in the config to
// keep struct size in one place).
typedef enum {
    DOWNLOAD_PENDING = 0,
    DOWNLOAD_IN_PROGRESS,
    DOWNLOAD_COMPLETED,
    DOWNLOAD_FAILED,
    DOWNLOAD_SKIPPED
} DownloadState;

// One queued download plus optional integrity hash.
typedef struct {
    char url[MAX_URL_LENGTH];
    DownloadState state;
    char error_msg[128];
    // SHA256 hex digest, lowercase, NUL-terminated. Empty string if not provided.
    char sha256_hex[SHA256_HEX_LEN + 1];
} DownloadItem;

// Application configuration (settings + URL list).
//
// Format of the config file is line-oriented:
//   - lines starting with `#` are comments
//   - blank lines are ignored
//   - `key=value` for known settings (unknown keys are ignored with no error)
//   - any other line is treated as a URL, optionally followed by
//     ` sha256:<hex>` for integrity checking
//
// Backwards compatible with the v1.0 format (extract_path/auto_retry/max_retries
// + bare URLs).
typedef struct {
    DownloadItem items[MAX_URLS];
    int count;

    // Settings
    char extract_path[MAX_PATH_LENGTH];
    bool auto_retry;
    int  max_retries;

    // Phase 2.10 — configurable limits, with safe defaults.
    int  max_urls;            // default 100 (= MAX_URLS)
    int  download_buffer_kb;  // default 128
    int  connect_timeout_s;   // default 30
} AppConfig;

// Read a configuration file into `cfg`. Defaults are applied first so an
// empty/missing file still yields a usable config.
//
// Returns the number of URLs read on success, or -1 if the file does not
// exist (or cannot be opened). The defaults are still applied to `cfg`
// in that case.
int read_config_file(const char* file_path, AppConfig* cfg);

// Apply defaults to `cfg`, leaving `count == 0` and no URLs.
void config_apply_defaults(AppConfig* cfg);

// Write a commented example configuration to `file_path`. Returns true on
// success.
bool create_example_config(const char* file_path);

#endif // CONFIG_H
