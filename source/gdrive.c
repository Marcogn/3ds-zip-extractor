#include "gdrive.h"

#include <stdio.h>
#include <string.h>

// Maximum size of a Google Drive file ID buffer used internally.
#define GDRIVE_MAX_ID_LEN 256

void convert_gdrive_url(const char* input_url, char* output_url, size_t output_size) {
    if (output_url == NULL || output_size == 0) {
        return;
    }
    output_url[0] = '\0';
    if (input_url == NULL) {
        return;
    }

    // Pass-through if not a Google Drive URL.
    if (strstr(input_url, "drive.google.com") == NULL) {
        strncpy(output_url, input_url, output_size - 1);
        output_url[output_size - 1] = '\0';
        return;
    }

    // Folder URLs are not downloadable as a single file.
    if (strstr(input_url, "/folders/") != NULL) {
        // Pass through unchanged; caller may emit a warning.
        strncpy(output_url, input_url, output_size - 1);
        output_url[output_size - 1] = '\0';
        return;
    }

    char file_id[GDRIVE_MAX_ID_LEN];
    file_id[0] = '\0';

    // Format: https://drive.google.com/file/d/FILE_ID/view
    const char* id_start = strstr(input_url, "/file/d/");
    if (id_start != NULL) {
        id_start += 8; // skip "/file/d/"
        const char* id_end = strchr(id_start, '/');
        size_t id_len = id_end ? (size_t)(id_end - id_start) : strlen(id_start);
        if (id_len > 0 && id_len < sizeof(file_id)) {
            memcpy(file_id, id_start, id_len);
            file_id[id_len] = '\0';
        }
    }

    // Format: ...?id=FILE_ID&... or ...?id=FILE_ID at end
    if (file_id[0] == '\0') {
        id_start = strstr(input_url, "id=");
        if (id_start != NULL) {
            id_start += 3; // skip "id="
            const char* id_end = strpbrk(id_start, "&/ \t\n\r");
            size_t id_len = id_end ? (size_t)(id_end - id_start) : strlen(id_start);
            if (id_len > 0 && id_len < sizeof(file_id)) {
                memcpy(file_id, id_start, id_len);
                file_id[id_len] = '\0';
            }
        }
    }

    if (file_id[0] != '\0') {
        // confirm=t bypasses the virus-scan warning for files >100MB.
        snprintf(output_url, output_size,
                 "https://drive.google.com/uc?export=download&id=%s&confirm=t",
                 file_id);
        return;
    }

    // Couldn't parse — fall back to the original URL.
    strncpy(output_url, input_url, output_size - 1);
    output_url[output_size - 1] = '\0';
}
