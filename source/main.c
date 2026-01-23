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

static ExtractData extract_data = {0};

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
        
        // Update display
        consoleClear();
        printf("\x1b[2;1HZip Extractor for 3DS");
        printf("\x1b[4;1H================================");
        printf("\x1b[6;1HDownloading...");
        printf("\x1b[8;1HProgress: %.2f MB / %.2f MB", 
               (data->downloaded + dlnow) / (1024.0 * 1024.0),
               (data->downloaded + dltotal) / (1024.0 * 1024.0));
        printf("\x1b[9;1HPercentage: %.1f%%", 
               ((data->downloaded + dlnow) * 100.0) / (data->downloaded + dltotal));
        printf("\x1b[11;1HPress B to cancel");
        
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    
    // Check if user wants to cancel
    hidScanInput();
    u32 kDown = hidKeysDown();
    if (kDown & KEY_B) {
        return 1; // Abort download
    }
    
    return 0;
}

// Download file with resume support
static Result download_file(const char* url, const char* output_path, bool* cancelled) {
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
    // Note: SSL verification is disabled due to certificate store limitations on 3DS
    // This is a known trade-off for 3DS homebrew applications
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "3DS-Zip-Extractor/1.0");
    
    // Resume support
    if (data.resume && data.downloaded > 0) {
        curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)data.downloaded);
    }
    
    res = curl_easy_perform(curl);
    
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
static Result extract_archive(const char* archive_path, const char* output_dir) {
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
        printf("\x1b[6;1HExtracting...");
        printf("\x1b[8;1HFiles extracted: %llu", extract_data.extracted_files);
        printf("\x1b[10;1HCurrent file:");
        printf("\x1b[11;1H%.40s", extract_data.current_file);
        
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
    ret = socInit((u32*)memalign(0x1000, 0x100000), 0x100000);
    if (ret != 0) {
        printf("socInit failed: 0x%08lX\n", ret);
        goto cleanup;
    }
    
    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);
    
    printf("\x1b[2;1HZip Extractor for 3DS");
    printf("\x1b[4;1H================================");
    printf("\x1b[6;1HReady to download and extract");
    printf("\x1b[8;1HUsage:");
    printf("\x1b[9;1H  Edit source code to set URL");
    printf("\x1b[10;1H  Recompile with your URL");
    printf("\x1b[12;1HDefault extract path:");
    printf("\x1b[13;1H  %s", DEFAULT_EXTRACT_PATH);
    printf("\x1b[16;1HPress A to start");
    printf("\x1b[17;1HPress START to exit");
    
    // Example URL - user should modify this
    const char* download_url = "https://example.com/file.zip";
    const char* extract_path = DEFAULT_EXTRACT_PATH;
    
    bool started = false;
    bool cancelled = false;
    
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        if (kDown & KEY_START) {
            break;
        }
        
        if ((kDown & KEY_A) && !started) {
            started = true;
            
            // Create extract directory
            mkdir("sdmc:/extracted", 0777);
            
            consoleClear();
            printf("\x1b[2;1HZip Extractor for 3DS");
            printf("\x1b[4;1H================================");
            printf("\x1b[6;1HStarting download...");
            printf("\x1b[8;1HURL: %s", download_url);
            
            gfxFlushBuffers();
            gfxSwapBuffers();
            gspWaitForVBlank();
            
            // Download file
            Result download_result = download_file(download_url, TEMP_DOWNLOAD_PATH, &cancelled);
            
            if (cancelled) {
                consoleClear();
                printf("\x1b[2;1HZip Extractor for 3DS");
                printf("\x1b[4;1H================================");
                printf("\x1b[6;1HDownload cancelled");
                printf("\x1b[8;1HPress START to exit");
                started = false;
                remove(TEMP_DOWNLOAD_PATH);
            } else if (download_result == 0) {
                // Extract archive
                Result extract_result = extract_archive(TEMP_DOWNLOAD_PATH, extract_path);
                
                if (extract_result == 0) {
                    consoleClear();
                    printf("\x1b[2;1HZip Extractor for 3DS");
                    printf("\x1b[4;1H================================");
                    printf("\x1b[6;1HExtraction complete!");
                    printf("\x1b[8;1HFiles extracted: %llu", extract_data.extracted_files);
                    printf("\x1b[10;1HOutput directory:");
                    printf("\x1b[11;1H  %s", extract_path);
                    printf("\x1b[14;1HPress START to exit");
                } else if (extract_result == -2) {
                    consoleClear();
                    printf("\x1b[2;1HZip Extractor for 3DS");
                    printf("\x1b[4;1H================================");
                    printf("\x1b[6;1HExtraction cancelled");
                    printf("\x1b[8;1HPress START to exit");
                } else {
                    consoleClear();
                    printf("\x1b[2;1HZip Extractor for 3DS");
                    printf("\x1b[4;1H================================");
                    printf("\x1b[6;1HExtraction failed");
                    printf("\x1b[8;1HPress START to exit");
                }
                
                // Clean up temp file
                remove(TEMP_DOWNLOAD_PATH);
                started = false;
            } else {
                consoleClear();
                printf("\x1b[2;1HZip Extractor for 3DS");
                printf("\x1b[4;1H================================");
                printf("\x1b[6;1HDownload failed");
                printf("\x1b[8;1HPress START to exit");
                started = false;
            }
        }
        
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    
    curl_global_cleanup();
    socExit();
    
cleanup:
    gfxExit();
    return 0;
}
