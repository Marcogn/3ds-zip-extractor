#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>  // strncasecmp (POSIX)

#define DEFAULT_EXTRACT_PATH "sdmc:/extracted/"

// --- Internal helpers ------------------------------------------------------

// Trim trailing CR/LF in place.
static void strip_eol(char* s, size_t* len_inout) {
    size_t len = *len_inout;
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
    *len_inout = len;
}

// Trim trailing ASCII whitespace in place.
static void rtrim(char* s) {
    size_t len = strlen(s);
    while (len > 0) {
        unsigned char c = (unsigned char)s[len - 1];
        if (c == ' ' || c == '\t') {
            s[--len] = '\0';
        } else {
            break;
        }
    }
}

// Skip a single UTF-8 BOM (EF BB BF) at the start of `s` if present.
static char* skip_bom(char* s) {
    if ((unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF) {
        return s + 3;
    }
    return s;
}

// Match `prefix` at the start of `s`. Returns pointer to value after the
// prefix, or NULL if not a match.
static const char* match_prefix(const char* s, const char* prefix) {
    size_t pl = strlen(prefix);
    if (strncmp(s, prefix, pl) == 0) {
        return s + pl;
    }
    return NULL;
}

// Parse a "true"/"false"/"1"/"0" boolean. Unknown values default to `dflt`.
static bool parse_bool(const char* v, bool dflt) {
    if (v == NULL) return dflt;
    if (strcmp(v, "true") == 0 || strcmp(v, "1") == 0) return true;
    if (strcmp(v, "false") == 0 || strcmp(v, "0") == 0) return false;
    return dflt;
}

// Clamp helper.
static int clamp_int(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// Split a URL line into URL and optional sha256 hex digest.
// Recognises the trailing ` sha256:<64hex>` token (case-insensitive prefix,
// hex digits required to be exactly 64). On match, the URL portion is
// truncated in `line` and `out_hash` is populated (lowercase). If no valid
// hash is found, `out_hash[0]` is left as NUL.
static void extract_sha256(char* line, char* out_hash) {
    out_hash[0] = '\0';

    // Find the last whitespace-separated token starting with sha256:
    // We scan for " sha256:" (case-insensitive) so the URL itself can
    // contain "sha256" without issue.
    char* p = line;
    char* match = NULL;
    while (*p) {
        if ((*p == ' ' || *p == '\t') &&
            (strncasecmp(p + 1, "sha256:", 7) == 0)) {
            match = p;
        }
        p++;
    }
    if (match == NULL) {
        return;
    }

    const char* hex = match + 1 + 7; // skip the space and "sha256:"
    // Validate that the remainder is exactly 64 hex chars (allow trailing ws).
    size_t hex_len = 0;
    while (hex[hex_len] != '\0' && hex[hex_len] != ' ' && hex[hex_len] != '\t') {
        if (!isxdigit((unsigned char)hex[hex_len])) {
            return; // not a clean hex token — leave URL untouched
        }
        hex_len++;
    }
    if (hex_len != SHA256_HEX_LEN) {
        return;
    }

    // Copy lowercase digest.
    for (size_t i = 0; i < SHA256_HEX_LEN; i++) {
        out_hash[i] = (char)tolower((unsigned char)hex[i]);
    }
    out_hash[SHA256_HEX_LEN] = '\0';

    // Truncate the line at the match (drop trailing spaces too).
    *match = '\0';
    rtrim(line);
}

// --- Public API ------------------------------------------------------------

void config_apply_defaults(AppConfig* cfg) {
    if (cfg == NULL) return;
    memset(cfg, 0, sizeof(*cfg));
    strncpy(cfg->extract_path, DEFAULT_EXTRACT_PATH, MAX_PATH_LENGTH - 1);
    cfg->extract_path[MAX_PATH_LENGTH - 1] = '\0';
    cfg->auto_retry = false;
    cfg->max_retries = 3;
    cfg->max_urls = MAX_URLS;
    cfg->download_buffer_kb = 128;
    cfg->connect_timeout_s = 30;
    cfg->count = 0;
}

int read_config_file(const char* file_path, AppConfig* cfg) {
    if (cfg == NULL) return -1;
    config_apply_defaults(cfg);

    FILE* file = fopen(file_path, "r");
    if (!file) {
        return -1;
    }

    char line[MAX_URL_LENGTH];
    bool first_line = true;

    while (fgets(line, sizeof(line), file) != NULL) {
        // Strip CR/LF.
        size_t len = strlen(line);
        bool line_was_truncated = (len == sizeof(line) - 1 && line[len - 1] != '\n');
        strip_eol(line, &len);

        // If the line exceeded MAX_URL_LENGTH, drain the rest of it so
        // subsequent fgets calls start at the next real line. We then keep
        // the truncated content as the URL/setting (it will be too long to
        // be useful, but won't overflow buffers).
        if (line_was_truncated) {
            int c;
            while ((c = fgetc(file)) != EOF && c != '\n') {
                /* drop */
            }
        }

        // Strip a leading BOM on the first line only.
        char* p = line;
        if (first_line) {
            p = skip_bom(p);
            first_line = false;
        }

        // Skip blank lines and comments.
        if (*p == '\0' || *p == '#') {
            continue;
        }

        // Settings (key=value) — order independent.
        const char* v;
        if ((v = match_prefix(p, "extract_path=")) != NULL) {
            strncpy(cfg->extract_path, v, MAX_PATH_LENGTH - 1);
            cfg->extract_path[MAX_PATH_LENGTH - 1] = '\0';
            continue;
        }
        if ((v = match_prefix(p, "auto_retry=")) != NULL) {
            cfg->auto_retry = parse_bool(v, false);
            continue;
        }
        if ((v = match_prefix(p, "max_retries=")) != NULL) {
            cfg->max_retries = clamp_int(atoi(v), 0, 10);
            continue;
        }
        if ((v = match_prefix(p, "max_urls=")) != NULL) {
            cfg->max_urls = clamp_int(atoi(v), 1, MAX_URLS);
            continue;
        }
        if ((v = match_prefix(p, "download_buffer_kb=")) != NULL) {
            cfg->download_buffer_kb = clamp_int(atoi(v), 4, 4096);
            continue;
        }
        if ((v = match_prefix(p, "connect_timeout_s=")) != NULL) {
            cfg->connect_timeout_s = clamp_int(atoi(v), 1, 600);
            continue;
        }

        // Otherwise it's a URL, possibly with ` sha256:HEX` suffix.
        if (cfg->count < cfg->max_urls && cfg->count < MAX_URLS) {
            DownloadItem* it = &cfg->items[cfg->count];
            extract_sha256(p, it->sha256_hex);
            // Defensive copy (truncate, no overflow).
            strncpy(it->url, p, MAX_URL_LENGTH - 1);
            it->url[MAX_URL_LENGTH - 1] = '\0';
            it->state = DOWNLOAD_PENDING;
            it->error_msg[0] = '\0';
            cfg->count++;
        }
        // Else: silently drop excess URLs (clamped by max_urls).
    }

    fclose(file);
    return cfg->count;
}

bool create_example_config(const char* file_path) {
    FILE* file = fopen(file_path, "w");
    if (!file) {
        return false;
    }

    fprintf(file, "# 3DS Archive Extractor - Configuration File\n");
    fprintf(file, "# Lines starting with # are comments\n");
    fprintf(file, "#\n");
    fprintf(file, "# Settings:\n");
    fprintf(file, "extract_path=sdmc:/extracted/\n");
    fprintf(file, "auto_retry=true\n");
    fprintf(file, "max_retries=3\n");
    fprintf(file, "#\n");
    fprintf(file, "# Optional limits (defaults shown):\n");
    fprintf(file, "# max_urls=100\n");
    fprintf(file, "# download_buffer_kb=128\n");
    fprintf(file, "# connect_timeout_s=30\n");
    fprintf(file, "#\n");
    fprintf(file, "# Add your URLs below (one per line):\n");
    fprintf(file, "# Example:\n");
    fprintf(file, "# https://example.com/file.zip\n");
    fprintf(file, "# https://example.com/file.zip sha256:<64-hex-digits>\n");
    fprintf(file, "# https://drive.google.com/file/d/FILE_ID/view\n");
    fprintf(file, "#\n");
    fprintf(file, "# Supported formats:\n");
    fprintf(file, "# ZIP, TAR, TAR.GZ, TAR.BZ2, TAR.XZ, TAR.ZSTD\n");
    fprintf(file, "# 7Z, RAR, GZIP, BZIP2, XZ, ZSTD\n");
    fprintf(file, "#\n");
    fprintf(file, "# Add your URLs here:\n");
    fprintf(file, "\n");

    fclose(file);
    return true;
}
