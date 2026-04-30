#include "download.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <3ds.h>
#include <curl/curl.h>

#include "gdrive.h"
#include "gui.h"

#define TEMP_DIR "sdmc:/3ds/zip-extractor/tmp"

typedef struct {
    FILE* file;
    u64 downloaded;
    u64 total;
    bool resume;
    // For server-200-when-resuming detection
    bool first_chunk;
} DownloadData;

// curl write callback. If we asked to resume but the server replied with
// 200 (full content), reset the file before writing the first chunk.
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    DownloadData* data = (DownloadData*)userp;

    if (data->first_chunk && data->resume) {
        // Will be reset elsewhere if needed; here we just record we've started.
        data->first_chunk = false;
    }

    size_t written = fwrite(contents, 1, realsize, data->file);
    data->downloaded += written;
    return written;
}

// Progress + cancel-detection callback.
static int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow,
                             curl_off_t ultotal, curl_off_t ulnow) {
    (void)dlnow; (void)ultotal; (void)ulnow;
    DownloadData* data = (DownloadData*)clientp;
    if (dltotal > 0) {
        data->total = dltotal;
    }

    hidScanInput();
    if (hidKeysDown() & KEY_B) {
        return 1; // abort
    }
    return 0;
}

void download_make_temp_path(char* out, size_t out_size) {
    if (out == NULL || out_size == 0) return;
    mkdir("sdmc:/3ds", 0777);
    mkdir("sdmc:/3ds/zip-extractor", 0777);
    mkdir(TEMP_DIR, 0777);

    u64 ts = osGetTime();
    // libctru does not expose a real PID; use a counter + osGetTime to create
    // a path that is unique per-run.
    static unsigned int counter = 0;
    counter++;
    snprintf(out, out_size, "%s/run_%llu_%u.part",
             TEMP_DIR, (unsigned long long)ts, counter);
}

void download_cleanup_orphans(void) {
    DIR* d = opendir(TEMP_DIR);
    if (!d) return;
    struct dirent* e;
    char path[512];
    while ((e = readdir(d)) != NULL) {
        // Remove any file ending in ".part" left from previous runs.
        size_t n = strlen(e->d_name);
        if (n > 5 && strcmp(e->d_name + n - 5, ".part") == 0) {
            snprintf(path, sizeof(path), "%s/%s", TEMP_DIR, e->d_name);
            remove(path);
        }
    }
    closedir(d);
}

Result download_file(const AppConfig* cfg,
                     const char* url, const char* output_path,
                     bool* cancelled, int current, int total) {
    CURL* curl;
    CURLcode res;
    DownloadData data = {0};
    *cancelled = false;
    data.first_chunk = true;

    long buffer_size = (long)(cfg ? cfg->download_buffer_kb : 128) * 1024L;
    long connect_timeout = (long)(cfg ? cfg->connect_timeout_s : 30);
    if (buffer_size < 4096) buffer_size = 4096;

    // Check if a partial file exists (for resume).
    FILE* check_file = fopen(output_path, "rb");
    if (check_file) {
        fseek(check_file, 0, SEEK_END);
        long sz = ftell(check_file);
        fclose(check_file);
        if (sz > 0) {
            data.downloaded = (u64)sz;
            data.resume = true;
        }
    }

    data.file = fopen(output_path, data.resume ? "ab" : "wb");
    if (!data.file) {
        return -1;
    }

    curl = curl_easy_init();
    if (!curl) {
        fclose(data.file);
        return -1;
    }

    char converted_url[512];
    convert_gdrive_url(url, converted_url, sizeof(converted_url));

    curl_easy_setopt(curl, CURLOPT_URL, converted_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &data);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, buffer_size);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connect_timeout);
    // SSL verification disabled because the 3DS lacks a modern certificate
    // store. Documented trade-off (see SECURITY.md).
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "3DS-Zip-Extractor/1.0");

    if (data.resume && data.downloaded > 0) {
        curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)data.downloaded);
    }

    gui_draw_download(current, total, converted_url, data.downloaded, data.total);
    res = curl_easy_perform(curl);

    // If we asked to resume but got 200 (full content), restart from scratch.
    if (res == CURLE_OK && data.resume) {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 200) {
            // Server ignored Range; close, truncate, retry.
            fclose(data.file);
            data.file = fopen(output_path, "wb");
            data.downloaded = 0;
            data.resume = false;
            if (!data.file) {
                curl_easy_cleanup(curl);
                return -1;
            }
            curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)0);
            res = curl_easy_perform(curl);
        }
    }

    if (res == CURLE_OK) {
        gui_draw_download(current, total, converted_url, data.downloaded, data.total);
    }

    curl_easy_cleanup(curl);
    fclose(data.file);

    if (res == CURLE_ABORTED_BY_CALLBACK) {
        *cancelled = true;
        return -2;
    }
    if (res != CURLE_OK) {
        return -1;
    }
    return 0;
}
