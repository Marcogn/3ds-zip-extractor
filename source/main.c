#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <3ds.h>
#include <curl/curl.h>
#include <archive.h>
#include <archive_entry.h>

#define DOWNLOAD_BUFFER_SIZE (128 * 1024)  // 128KB buffer
#define DEFAULT_EXTRACT_PATH "sdmc:/extracted/"
#define TEMP_DOWNLOAD_PATH "sdmc:/temp_download.tmp"
#define CONFIG_FILE_PATH "sdmc:/3ds/zip-extractor/config.txt"
#define MAX_URLS 50
#define MAX_URL_LENGTH 512
#define MAX_PATH_LENGTH 256

// Download states for queue management
typedef enum {
    DOWNLOAD_PENDING,
    DOWNLOAD_IN_PROGRESS,
    DOWNLOAD_COMPLETED,
    DOWNLOAD_FAILED,
    DOWNLOAD_SKIPPED
} DownloadState;

typedef struct {
    FILE* file;
    u64 downloaded;
    u64 total;
    bool resume;
} DownloadData;

typedef struct {
    u64 extracted_files;
    u64 current_size;
    char current_file[256];
} ExtractData;

typedef struct {
    char url[MAX_URL_LENGTH];
    DownloadState state;
    char error_msg[128];
} DownloadItem;

typedef struct {
    DownloadItem items[MAX_URLS];
    int count;
    char extract_path[MAX_PATH_LENGTH];
    bool auto_retry;
    int max_retries;
} DownloadQueue;

static ExtractData extract_data = {0};

// Function to read configuration file with settings and URLs
static int read_config_file(const char* file_path, DownloadQueue* queue) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        return -1;
    }
    
    // Set defaults
    strncpy(queue->extract_path, DEFAULT_EXTRACT_PATH, MAX_PATH_LENGTH - 1);
    queue->extract_path[MAX_PATH_LENGTH - 1] = '\0';
    queue->auto_retry = false;
    queue->max_retries = 3;
    queue->count = 0;
    
    char line[MAX_URL_LENGTH];
    
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove trailing newline and carriage return
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
            len--;
        }
        
        // Skip empty lines and comments
        if (len == 0 || line[0] == '#') {
            continue;
        }
        
        // Check for settings (format: setting=value)
        if (strstr(line, "extract_path=") == line) {
            strncpy(queue->extract_path, line + 13, MAX_PATH_LENGTH - 1);
            queue->extract_path[MAX_PATH_LENGTH - 1] = '\0';
            continue;
        }
        
        if (strstr(line, "auto_retry=") == line) {
            queue->auto_retry = (strcmp(line + 11, "true") == 0 || strcmp(line + 11, "1") == 0);
            continue;
        }
        
        if (strstr(line, "max_retries=") == line) {
            queue->max_retries = atoi(line + 12);
            if (queue->max_retries < 0) queue->max_retries = 0;
            if (queue->max_retries > 10) queue->max_retries = 10;
            continue;
        }
        
        // Otherwise, treat as URL
        if (queue->count < MAX_URLS) {
            strncpy(queue->items[queue->count].url, line, MAX_URL_LENGTH - 1);
            queue->items[queue->count].url[MAX_URL_LENGTH - 1] = '\0';
            queue->items[queue->count].state = DOWNLOAD_PENDING;
            queue->items[queue->count].error_msg[0] = '\0';
            queue->count++;
        }
    }
    
    fclose(file);
    return queue->count;
}

// Display queue status
static void display_queue_status(DownloadQueue* queue, int current_page) {
    consoleClear();
    printf("\x1b[1;1HZip Extractor - Queue Status");
    printf("\x1b[2;1H================================");
    
    int items_per_page = 12;
    int start = current_page * items_per_page;
    int end = start + items_per_page;
    if (end > queue->count) end = queue->count;
    
    printf("\x1b[4;1HShowing %d-%d of %d", start + 1, end, queue->count);
    
    for (int i = start; i < end; i++) {
        int line = 6 + (i - start);
        const char* state_str = "?";
        
        switch (queue->items[i].state) {
            case DOWNLOAD_PENDING: state_str = "[ ]"; break;
            case DOWNLOAD_IN_PROGRESS: state_str = "[>]"; break;
            case DOWNLOAD_COMPLETED: state_str = "[✓]"; break;
            case DOWNLOAD_FAILED: state_str = "[X]"; break;
            case DOWNLOAD_SKIPPED: state_str = "[-]"; break;
        }
        
        printf("\x1b[%d;1H%s %.38s", line, state_str, queue->items[i].url);
    }
    
    int total_pages = (queue->count + items_per_page - 1) / items_per_page;
    printf("\x1b[20;1HPage %d/%d", current_page + 1, total_pages);
    printf("\x1b[22;1HL/R: Change page  Y: Skip failed");
    printf("\x1b[23;1HA: Continue  B: Back  START: Exit");
}

// Function to read URLs from configuration file (legacy support)
static int read_urls_from_file(const char* file_path, UrlList* url_list) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        return -1;
    }
    
    url_list->count = 0;
    char line[MAX_URL_LENGTH];
    
    while (fgets(line, sizeof(line), file) != NULL && url_list->count < MAX_URLS) {
        // Remove trailing newline and carriage return
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
            len--;
        }
        
        // Skip empty lines and comments (lines starting with #)
        if (len == 0 || line[0] == '#') {
            continue;
        }
        
        // Copy URL to list
        strncpy(url_list->urls[url_list->count], line, MAX_URL_LENGTH - 1);
        url_list->urls[url_list->count][MAX_URL_LENGTH - 1] = '\0';
        url_list->count++;
    }
    
    fclose(file);
    return url_list->count;
}

// Function to convert Google Drive URLs to direct download links
static void convert_gdrive_url(const char* input_url, char* output_url, size_t output_size) {
    // Check if it's a Google Drive URL
    if (strstr(input_url, "drive.google.com") != NULL) {
        // Extract file ID from various Google Drive URL formats
        const char* file_id = NULL;
        char* id_start = NULL;
        
        // Format: https://drive.google.com/file/d/FILE_ID/view
        if ((id_start = strstr(input_url, "/file/d/")) != NULL) {
            id_start += 8; // Skip "/file/d/"
            char* id_end = strchr(id_start, '/');
            if (id_end) {
                size_t id_len = id_end - id_start;
                char file_id_buf[256];
                strncpy(file_id_buf, id_start, id_len);
                file_id_buf[id_len] = '\0';
                snprintf(output_url, output_size, 
                         "https://drive.google.com/uc?export=download&id=%s", 
                         file_id_buf);
                return;
            }
        }
        
        // Format: https://drive.google.com/open?id=FILE_ID
        if ((id_start = strstr(input_url, "id=")) != NULL) {
            id_start += 3; // Skip "id="
            char* id_end = strchr(id_start, '&');
            if (id_end) {
                size_t id_len = id_end - id_start;
                char file_id_buf[256];
                strncpy(file_id_buf, id_start, id_len);
                file_id_buf[id_len] = '\0';
                snprintf(output_url, output_size, 
                         "https://drive.google.com/uc?export=download&id=%s", 
                         file_id_buf);
                return;
            } else {
                // ID is at the end of URL
                snprintf(output_url, output_size, 
                         "https://drive.google.com/uc?export=download&id=%s", 
                         id_start);
                return;
            }
        }
    }
    
    // Not a Google Drive URL or couldn't parse, use original
    strncpy(output_url, input_url, output_size - 1);
    output_url[output_size - 1] = '\0';
}

// Callback for writing downloaded data
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    DownloadData* data = (DownloadData*)userp;
    
    size_t written = fwrite(contents, 1, realsize, data->file);
    data->downloaded += written;
    
    return written;
}

// Callback for download progress
static int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow,
                            curl_off_t ultotal, curl_off_t ulnow) {
    DownloadData* data = (DownloadData*)clientp;
    
    if (dltotal > 0) {
        data->total = dltotal;
    }
    
    // Check if user wants to cancel
    hidScanInput();
    u32 kDown = hidKeysDown();
    if (kDown & KEY_B) {
        return 1; // Abort download
    }
    
    return 0;
}

// Update download progress display
static void update_download_display(int current, int total, DownloadData* data, const char* url) {
    consoleClear();
    printf("\x1b[2;1HZip Extractor for 3DS");
    printf("\x1b[4;1H================================");
    if (total > 1) {
        printf("\x1b[6;1HDownloading file %d of %d", current, total);
    } else {
        printf("\x1b[6;1HDownloading...");
    }
    printf("\x1b[8;1HURL: %.45s", url);
    if (data->total > 0) {
        printf("\x1b[10;1HProgress: %.2f MB / %.2f MB", 
               (data->downloaded) / (1024.0 * 1024.0),
               (data->total) / (1024.0 * 1024.0));
        printf("\x1b[11;1HPercentage: %.1f%%", 
               (data->downloaded * 100.0) / data->total);
    }
    printf("\x1b[13;1HPress B to cancel");
    
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}

// Download file with resume support
static Result download_file(const char* url, const char* output_path, bool* cancelled, int current, int total) {
    CURL* curl;
    CURLcode res;
    DownloadData data = {0};
    *cancelled = false;
    
    // Check if partial file exists
    FILE* check_file = fopen(output_path, "rb");
    if (check_file) {
        fseek(check_file, 0, SEEK_END);
        data.downloaded = ftell(check_file);
        fclose(check_file);
        data.resume = true;
    }
    
    // Open file for appending or writing
    data.file = fopen(output_path, data.resume ? "ab" : "wb");
    if (!data.file) {
        printf("Failed to open output file\n");
        return -1;
    }
    
    curl = curl_easy_init();
    if (!curl) {
        fclose(data.file);
        return -1;
    }
    
    // Convert Google Drive URLs if needed
    char converted_url[512];
    convert_gdrive_url(url, converted_url, sizeof(converted_url));
    
    curl_easy_setopt(curl, CURLOPT_URL, converted_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &data);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, DOWNLOAD_BUFFER_SIZE);
    // Note: SSL verification is disabled due to certificate store limitations on 3DS
    // This is a known trade-off for 3DS homebrew applications
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "3DS-Zip-Extractor/1.0");
    
    // Resume support
    if (data.resume && data.downloaded > 0) {
        curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)data.downloaded);
    }
    
    // Initial display update
    update_download_display(current, total, &data, converted_url);
    
    // Perform download with periodic display updates
    res = curl_easy_perform(curl);
    
    // Final display update
    if (res == CURLE_OK) {
        update_download_display(current, total, &data, converted_url);
    }
    
    curl_easy_cleanup(curl);
    fclose(data.file);
    
    if (res == CURLE_ABORTED_BY_CALLBACK) {
        *cancelled = true;
        return -2;
    }
    
    if (res != CURLE_OK) {
        printf("Download failed: %s\n", curl_easy_strerror(res));
        return -1;
    }
    
    return 0;
}

// Extract archive with progress tracking
static Result extract_archive(const char* archive_path, const char* output_dir, int current, int total) {
    struct archive* a;
    struct archive* ext;
    struct archive_entry* entry;
    int r;
    
    extract_data.extracted_files = 0;
    extract_data.current_size = 0;
    
    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);
    
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_TIME);
    archive_write_disk_set_standard_lookup(ext);
    
    if ((r = archive_read_open_filename(a, archive_path, 10240))) {
        printf("Failed to open archive: %s\n", archive_error_string(a));
        archive_read_free(a);
        archive_write_free(ext);
        return -1;
    }
    
    consoleClear();
    printf("\x1b[2;1HZip Extractor for 3DS");
    printf("\x1b[4;1H================================");
    printf("\x1b[6;1HExtracting archive...\n");
    
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char* current_file = archive_entry_pathname(entry);
        char full_path[512];
        
        snprintf(full_path, sizeof(full_path), "%s%s", output_dir, current_file);
        archive_entry_set_pathname(entry, full_path);
        
        strncpy(extract_data.current_file, current_file, sizeof(extract_data.current_file) - 1);
        extract_data.current_file[sizeof(extract_data.current_file) - 1] = '\0';
        extract_data.current_size = archive_entry_size(entry);
        
        // Update display
        consoleClear();
        printf("\x1b[2;1HZip Extractor for 3DS");
        printf("\x1b[4;1H================================");
        if (total > 1) {
            printf("\x1b[6;1HExtracting archive %d of %d", current, total);
        } else {
            printf("\x1b[6;1HExtracting...");
        }
        printf("\x1b[8;1HFiles extracted: %llu", extract_data.extracted_files);
        printf("\x1b[10;1HCurrent file:");
        printf("\x1b[11;1H%.40s", extract_data.current_file);
        printf("\x1b[13;1HPress B to cancel");
        
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        
        r = archive_write_header(ext, entry);
        if (r == ARCHIVE_OK) {
            const void* buff;
            size_t size;
            la_int64_t offset;
            
            while ((r = archive_read_data_block(a, &buff, &size, &offset)) == ARCHIVE_OK) {
                if (archive_write_data_block(ext, buff, size, offset) != ARCHIVE_OK) {
                    printf("Write error: %s\n", archive_error_string(ext));
                    break;
                }
                
                // Check for cancel
                hidScanInput();
                u32 kDown = hidKeysDown();
                if (kDown & KEY_B) {
                    archive_read_free(a);
                    archive_write_free(ext);
                    return -2;
                }
            }
            
            if (r != ARCHIVE_EOF) {
                printf("Read error: %s\n", archive_error_string(a));
            }
            
            archive_write_finish_entry(ext);
        }
        
        extract_data.extracted_files++;
    }
    
    archive_read_free(a);
    archive_write_free(ext);
    
    return 0;
}

int main(int argc, char** argv) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    
    // Initialize networking
    Result ret = 0;
    u32* socMemory = (u32*)memalign(0x1000, 0x100000);
    if (!socMemory) {
        printf("Failed to allocate socket memory\n");
        goto cleanup;
    }
    
    ret = socInit(socMemory, 0x100000);
    if (ret != 0) {
        printf("socInit failed: 0x%08lX\n", ret);
        free(socMemory);
        goto cleanup;
    }
    
    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);
    
    const char* extract_path = DEFAULT_EXTRACT_PATH;
    DownloadQueue queue = {0};
    
    // Create config directory if it doesn't exist
    ret = mkdir("sdmc:/3ds", 0777);
    ret = mkdir("sdmc:/3ds/zip-extractor", 0777);
    // Ignore errors - directories may already exist
    
    // Try to read configuration file
    int url_count = read_config_file(CONFIG_FILE_PATH, &queue);
    if (url_count > 0) {
        extract_path = queue.extract_path;
    }
    
    printf("\x1b[2;1HZip Extractor for 3DS");
    printf("\x1b[4;1H================================");
    
    if (url_count > 0) {
        printf("\x1b[6;1HLoaded %d URL(s) from config", url_count);
        printf("\x1b[8;1HConfig file:");
        printf("\x1b[9;1H  %s", CONFIG_FILE_PATH);
        printf("\x1b[11;1HExtract path:");
        printf("\x1b[12;1H  %s", extract_path);
        if (queue.auto_retry) {
            printf("\x1b[13;1HAuto-retry: ON (max %d)", queue.max_retries);
        }
        printf("\x1b[15;1HPress A to start downloads");
        printf("\x1b[16;1HPress X to view queue");
        printf("\x1b[17;1HPress START to exit");
    } else {
        printf("\x1b[6;1HNo config file found!");
        printf("\x1b[8;1HPlease create:");
        printf("\x1b[9;1H  %s", CONFIG_FILE_PATH);
        printf("\x1b[11;1HAdd URLs (one per line) or");
        printf("\x1b[12;1Huse old format at:");
        printf("\x1b[13;1H  sdmc:/3ds/zip-extractor/urls.txt");
        printf("\x1b[15;1HExample config.txt:");
        printf("\x1b[16;1H  extract_path=/extracted/");
        printf("\x1b[17;1H  auto_retry=true");
        printf("\x1b[18;1H  https://example.com/file.zip");
        printf("\x1b[20;1HPress START to exit");
    }
    
    bool started = false;
    bool cancelled = false;
    bool show_queue = false;
    int queue_page = 0;
    
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        if (kDown & KEY_START) {
            break;
        }
        
        // Queue navigation
        if (show_queue) {
            int total_pages = (url_count + 11) / 12;
            
            if ((kDown & KEY_R) && queue_page < total_pages - 1) {
                queue_page++;
                display_queue_status(&queue, queue_page);
            }
            
            if ((kDown & KEY_L) && queue_page > 0) {
                queue_page--;
                display_queue_status(&queue, queue_page);
            }
            
            if (kDown & KEY_Y) {
                // Skip all failed downloads
                for (int i = 0; i < url_count; i++) {
                    if (queue.items[i].state == DOWNLOAD_FAILED) {
                        queue.items[i].state = DOWNLOAD_SKIPPED;
                    }
                }
                display_queue_status(&queue, queue_page);
            }
            
            if (kDown & KEY_B) {
                show_queue = false;
                queue_page = 0;
                consoleClear();
                printf("\x1b[2;1HZip Extractor for 3DS");
                printf("\x1b[4;1H================================");
                printf("\x1b[6;1HLoaded %d URL(s) from config", url_count);
                printf("\x1b[8;1HConfig file:");
                printf("\x1b[9;1H  %s", CONFIG_FILE_PATH);
                printf("\x1b[11;1HExtract path:");
                printf("\x1b[12;1H  %s", extract_path);
                if (queue.auto_retry) {
                    printf("\x1b[13;1HAuto-retry: ON (max %d)", queue.max_retries);
                }
                printf("\x1b[15;1HPress A to start downloads");
                printf("\x1b[16;1HPress X to view queue");
                printf("\x1b[17;1HPress START to exit");
            }
            
            if ((kDown & KEY_A) && !started) {
                show_queue = false;
                started = true;
                // Continue to processing below
            }
        }
        
        // Show queue
        if ((kDown & KEY_X) && url_count > 0 && !started && !show_queue) {
            show_queue = true;
            queue_page = 0;
            display_queue_status(&queue, queue_page);
        }
        
        // Start processing
        if ((kDown & KEY_A) && url_count > 0 && !started && !show_queue) {
            started = true;
        }
        
        if (started) {
            // Create extract directory
            mkdir(extract_path, 0777);
            
            int successful = 0;
            int failed = 0;
            int skipped = 0;
            u64 total_files_extracted = 0;
            
            // Process each URL in queue
            for (int i = 0; i < url_count && !cancelled; i++) {
                // Skip items that are skipped or already completed
                if (queue.items[i].state == DOWNLOAD_SKIPPED) {
                    skipped++;
                    continue;
                }
                if (queue.items[i].state == DOWNLOAD_COMPLETED) {
                    successful++;
                    continue;
                }
                
                queue.items[i].state = DOWNLOAD_IN_PROGRESS;
                
                int retries = 0;
                bool download_success = false;
                
                do {
                    consoleClear();
                    printf("\x1b[2;1HZip Extractor for 3DS");
                    printf("\x1b[4;1H================================");
                    printf("\x1b[6;1HProcessing file %d of %d", i + 1, url_count);
                    if (retries > 0) {
                        printf("\x1b[7;1HRetry attempt %d/%d", retries, queue.max_retries);
                    }
                    printf("\x1b[9;1HStarting download...");
                    
                    gfxFlushBuffers();
                    gfxSwapBuffers();
                    gspWaitForVBlank();
                    
                    // Download file
                    Result download_result = download_file(queue.items[i].url, TEMP_DOWNLOAD_PATH, &cancelled, i + 1, url_count);
                    
                    if (cancelled) {
                        break;
                    }
                    
                    if (download_result == 0) {
                        // Extract archive
                        Result extract_result = extract_archive(TEMP_DOWNLOAD_PATH, extract_path, i + 1, url_count);
                        
                        if (extract_result == 0) {
                            queue.items[i].state = DOWNLOAD_COMPLETED;
                            successful++;
                            total_files_extracted += extract_data.extracted_files;
                            download_success = true;
                        } else if (extract_result == -2) {
                            cancelled = true;
                            break;
                        } else {
                            strncpy(queue.items[i].error_msg, "Extraction failed", sizeof(queue.items[i].error_msg) - 1);
                        }
                        
                        // Clean up temp file
                        remove(TEMP_DOWNLOAD_PATH);
                    } else {
                        strncpy(queue.items[i].error_msg, "Download failed", sizeof(queue.items[i].error_msg) - 1);
                    }
                    
                    if (!download_success && queue.auto_retry && retries < queue.max_retries) {
                        retries++;
                        // Wait a bit before retry
                        svcSleepThread(2000000000LL); // 2 seconds
                    } else {
                        break;
                    }
                    
                } while (!download_success && retries <= queue.max_retries);
                
                if (!download_success && !cancelled) {
                    queue.items[i].state = DOWNLOAD_FAILED;
                    failed++;
                }
            }
            
            // Show summary
            consoleClear();
            printf("\x1b[2;1HZip Extractor for 3DS");
            printf("\x1b[4;1H================================");
            
            if (cancelled) {
                printf("\x1b[6;1HOperation cancelled!");
            } else {
                printf("\x1b[6;1HAll downloads complete!");
            }
            
            printf("\x1b[8;1HSummary:");
            printf("\x1b[9;1H  Successful: %d", successful);
            printf("\x1b[10;1H  Failed: %d", failed);
            printf("\x1b[11;1H  Skipped: %d", skipped);
            printf("\x1b[12;1H  Total files extracted: %llu", total_files_extracted);
            printf("\x1b[14;1HOutput directory:");
            printf("\x1b[15;1H  %s", extract_path);
            
            if (failed > 0) {
                printf("\x1b[17;1HPress X to view failed items");
                printf("\x1b[18;1HPress A to retry failed");
            }
            printf("\x1b[19;1HPress START to exit");
            
            started = false;
            
            // Wait for user input
            while (aptMainLoop()) {
                hidScanInput();
                u32 kDown2 = hidKeysDown();
                
                if (kDown2 & KEY_START) {
                    goto exit_loop;
                }
                
                if ((kDown2 & KEY_X) && failed > 0) {
                    show_queue = true;
                    queue_page = 0;
                    display_queue_status(&queue, queue_page);
                    break;
                }
                
                if ((kDown2 & KEY_A) && failed > 0) {
                    // Reset failed items to pending and restart
                    for (int i = 0; i < url_count; i++) {
                        if (queue.items[i].state == DOWNLOAD_FAILED) {
                            queue.items[i].state = DOWNLOAD_PENDING;
                        }
                    }
                    started = true;
                    break;
                }
                
                gfxFlushBuffers();
                gfxSwapBuffers();
                gspWaitForVBlank();
            }
        }
        
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    
exit_loop:
    
    curl_global_cleanup();
    socExit();
    free(socMemory);
    
cleanup:
    gfxExit();
    return 0;
}
