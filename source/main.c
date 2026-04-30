#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>
#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <curl/curl.h>

#include "archive_extractor.h"
#include "config.h"
#include "download.h"
#include "file_browser.h"
#include "gui.h"
#include "integrity.h"
#include "led.h"
#include "queue.h"

#define CONFIG_FILE_PATH "sdmc:/3ds/zip-extractor/config.txt"

// Global GUI context for hybrid rendering.
static GUI g_gui = {0};
static bool g_use_gui = false;

// Tracks the currently-extracting archive for the GUI callback.
typedef struct {
    u64 extracted_files;
    char current_file[256];
} ExtractData;
static ExtractData extract_data = {0};

// --- Sleep mode -----------------------------------------------------------

static void enable_sleep_mode(void) {
    APT_SetAppCpuTimeLimit(30);  // 30% CPU for background activity.
}

static void restore_sleep_mode(void) {
    APT_SetAppCpuTimeLimit(0);
}

// --- Extraction adapter ---------------------------------------------------

static bool extraction_progress_callback(int file_count, const char* current_file, void* user_data) {
    (void)user_data;
    extract_data.extracted_files = file_count;
    strncpy(extract_data.current_file, current_file, sizeof(extract_data.current_file) - 1);
    extract_data.current_file[sizeof(extract_data.current_file) - 1] = '\0';

    gui_draw_extraction(current_file, file_count);

    hidScanInput();
    if (hidKeysDown() & KEY_B) {
        return true;  // cancel
    }
    return false;
}

// Extract `archive_path` into `output_dir`. Returns 0 on success, -1 on
// hard error, -2 if cancelled by user.
static Result extract_archive(const char* archive_path, const char* output_dir) {
    extract_data.extracted_files = 0;
    extract_data.current_file[0] = '\0';

    ArchiveType type = detect_archive_type(archive_path);
    const char* type_name = get_archive_type_name(type);

    char msg[64];
    snprintf(msg, sizeof(msg), "Extracting %s archive...", type_name);
    gui_draw_status("Archive Extractor for 3DS", msg);

    if (type == ARCHIVE_UNKNOWN) {
        gui_draw_error("Unsupported Format", "ZIP, TAR, 7Z, RAR supported");
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & KEY_A) break;
            gui_draw_error("Unsupported Format", "Press A to continue");
        }
        return -1;
    }

    int file_count = extract_archive_libarchive(archive_path, output_dir,
                                                 extraction_progress_callback, NULL);
    remove(archive_path);

    if (file_count == -4) {
        gui_draw_status("Archive Extractor for 3DS", "Extraction cancelled!");
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & KEY_A) break;
            gui_draw_status("Cancelled", "Press A to continue");
        }
        return -2;
    }
    if (file_count < 0) {
        char err_msg[64];
        snprintf(err_msg, sizeof(err_msg), "Extraction error: %d", file_count);
        gui_draw_error("Extraction Failed", err_msg);
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & KEY_A) break;
            gui_draw_error("Error", "Press A to continue");
        }
        return -1;
    }

    led_notification_green();
    gui_draw_status("Extraction Complete!", "Press A to continue");
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_A) break;
    }
    return 0;
}

// --- Main -----------------------------------------------------------------

int main(int argc, char** argv) {
    (void)argc; (void)argv;

    gfxInitDefault();
    g_use_gui = gui_init(&g_gui);
    if (!g_use_gui) {
        gfxExit();
        return 1;
    }

    gui_draw_status("Archive Extractor for 3DS", "Initializing...");
    ptmuInit();
    enable_sleep_mode();

    gui_draw_status("Archive Extractor for 3DS", "Initializing network...");
    u32* socMemory = (u32*)memalign(0x1000, 0x100000);
    if (!socMemory) {
        gui_draw_error("Error", "Failed to allocate socket memory!");
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & KEY_START) break;
        }
        gui_cleanup(&g_gui);
        ptmuExit();
        gfxExit();
        return 1;
    }

    if (socInit(socMemory, 0x100000) != 0) {
        gui_draw_error("Network Error", "Failed to initialize network!");
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & KEY_START) break;
        }
        free(socMemory);
        gui_cleanup(&g_gui);
        ptmuExit();
        gfxExit();
        return 1;
    }

    gui_draw_status("Archive Extractor for 3DS", "Initializing downloads...");
    curl_global_init(CURL_GLOBAL_ALL);

    // Heap-allocated to keep large structures off the small 3DS stack.
    DownloadQueue* queue = (DownloadQueue*)calloc(1, sizeof(DownloadQueue));
    if (!queue) {
        gui_draw_error("Memory Error", "Failed to allocate queue!");
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & KEY_START) break;
        }
        curl_global_cleanup();
        socExit();
        free(socMemory);
        gui_cleanup(&g_gui);
        ptmuExit();
        gfxExit();
        return 1;
    }

    gui_draw_status("Archive Extractor for 3DS", "Loading...");
    mkdir("sdmc:/3ds", 0777);
    mkdir("sdmc:/3ds/zip-extractor", 0777);

    // Sweep up any orphaned `.part` files from previous runs.
    download_cleanup_orphans();

    int url_count = read_config_file(CONFIG_FILE_PATH, queue);

    // First-run experience: create an example config and ask user to edit.
    if (url_count < 0) {
        gui_draw_status("Archive Extractor for 3DS", "Config not found! Creating...");
        for (int i = 0; i < 30; i++) gspWaitForVBlank();

        if (create_example_config(CONFIG_FILE_PATH)) {
            while (aptMainLoop()) {
                hidScanInput();
                if (hidKeysDown() & KEY_START) break;
                gui_draw_status("Config Created",
                                "Edit sdmc:/3ds/zip-extractor/config.txt and restart.");
            }
        } else {
            while (aptMainLoop()) {
                hidScanInput();
                if (hidKeysDown() & KEY_START) break;
                gui_draw_error("Cannot Create Config", "SD card read-only? Press START");
            }
        }
        goto exit_loop;
    }

    const char* extract_path = (url_count > 0) ? queue->extract_path : "sdmc:/extracted/";

    gui_draw_main_menu(url_count, CONFIG_FILE_PATH, extract_path,
                       queue->auto_retry, queue->max_retries);

    bool started = false;
    bool cancelled = false;
    bool show_queue = false;
    bool show_browser = false;
    int queue_page = 0;

    FileBrowser* browser = (FileBrowser*)calloc(1, sizeof(FileBrowser));
    if (!browser) {
        gui_draw_error("Warning", "File browser disabled (memory)");
    }
    int browser_scroll = 0;

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;

        // Default render: main menu.
        if (!started && !show_queue && !show_browser) {
            gui_draw_main_menu(url_count, CONFIG_FILE_PATH, extract_path,
                               queue->auto_retry, queue->max_retries);
        }

        // ----- File browser -----
        if (show_browser && browser) {
            if (kDown & KEY_UP) {
                if (browser->selected > 0) {
                    browser->selected--;
                    if (browser->selected < browser_scroll) browser_scroll = browser->selected;
                }
            }
            if (kDown & KEY_DOWN) {
                if (browser->selected < browser->count - 1) {
                    browser->selected++;
                    if (browser->selected >= browser_scroll + 14) {
                        browser_scroll = browser->selected - 13;
                    }
                }
                display_file_browser(browser, browser_scroll);
            }
            if (kDown & KEY_A) {
                if (browser->entries[browser->selected].is_directory) {
                    if (strcmp(browser->entries[browser->selected].name, "..") == 0) {
                        char* last_slash = strrchr(browser->current_path, '/');
                        if (last_slash != NULL && last_slash != browser->current_path) {
                            *last_slash = '\0';
                            if (strncmp(browser->current_path, "sdmc:", 5) == 0
                                && strlen(browser->current_path) == 5) {
                                strcat(browser->current_path, "/");
                            }
                        }
                    } else {
                        size_t path_len = strlen(browser->current_path);
                        size_t name_len = strlen(browser->entries[browser->selected].name);
                        if (path_len + name_len + 2 < MAX_PATH_LENGTH) {
                            if (browser->current_path[path_len - 1] != '/') {
                                browser->current_path[path_len++] = '/';
                                browser->current_path[path_len] = '\0';
                            }
                            strncat(browser->current_path,
                                    browser->entries[browser->selected].name,
                                    MAX_PATH_LENGTH - path_len - 1);
                        }
                    }
                    load_directory(browser);
                    browser_scroll = 0;
                    display_file_browser(browser, browser_scroll);
                }
            }
            if (kDown & KEY_Y) {
                strncpy(queue->extract_path, browser->current_path, MAX_PATH_LENGTH - 1);
                queue->extract_path[MAX_PATH_LENGTH - 1] = '\0';
                if (queue->extract_path[strlen(queue->extract_path) - 1] != '/') {
                    strcat(queue->extract_path, "/");
                }
                extract_path = queue->extract_path;
                show_browser = false;
            }
            if (kDown & KEY_B) {
                show_browser = false;
            }
        }

        // Open file browser.
        if ((kDown & KEY_SELECT) && url_count > 0 && !started && !show_queue && !show_browser && browser) {
            show_browser = true;
            init_file_browser(browser, "sdmc:/");
            load_directory(browser);
            browser_scroll = 0;
            display_file_browser(browser, browser_scroll);
        }

        // ----- Queue view -----
        if (show_queue) {
            int total_pages = (url_count + 11) / 12;
            if ((kDown & KEY_R) && queue_page < total_pages - 1) {
                queue_page++;
                display_queue_status(queue, queue_page);
            }
            if ((kDown & KEY_L) && queue_page > 0) {
                queue_page--;
                display_queue_status(queue, queue_page);
            }
            if (kDown & KEY_Y) {
                for (int i = 0; i < url_count; i++) {
                    if (queue->items[i].state == DOWNLOAD_FAILED) {
                        queue->items[i].state = DOWNLOAD_SKIPPED;
                    }
                }
                display_queue_status(queue, queue_page);
            }
            if (kDown & KEY_B) {
                show_queue = false;
                queue_page = 0;
            }
            if ((kDown & KEY_A) && !started) {
                show_queue = false;
                started = true;
            }
        }
        if ((kDown & KEY_X) && url_count > 0 && !started && !show_queue) {
            show_queue = true;
            queue_page = 0;
            display_queue_status(queue, queue_page);
        }

        // ----- Start processing -----
        if ((kDown & KEY_A) && url_count > 0 && !started && !show_queue) {
            started = true;
        }

        if (started) {
            mkdir(extract_path, 0777);

            int successful = 0;
            int failed = 0;
            int skipped = 0;
            u64 total_files_extracted = 0;

            for (int i = 0; i < url_count && !cancelled; i++) {
                if (queue->items[i].state == DOWNLOAD_SKIPPED) { skipped++;    continue; }
                if (queue->items[i].state == DOWNLOAD_COMPLETED) { successful++; continue; }

                queue->items[i].state = DOWNLOAD_IN_PROGRESS;
                int retries = 0;
                bool download_success = false;

                // Per-URL temp path so concurrent runs / aborts don't collide.
                char temp_path[160];
                download_make_temp_path(temp_path, sizeof(temp_path));

                do {
                    Result rc = download_file(queue, queue->items[i].url, temp_path,
                                              &cancelled, i + 1, url_count);
                    if (cancelled) break;

                    if (rc == 0) {
                        led_notification_pink();

                        // Optional integrity check.
                        if (queue->items[i].sha256_hex[0] != '\0' &&
                            !verify_integrity(temp_path, queue->items[i].sha256_hex)) {
                            strncpy(queue->items[i].error_msg, "SHA256 mismatch",
                                    sizeof(queue->items[i].error_msg) - 1);
                            remove(temp_path);
                        } else {
                            Result er = extract_archive(temp_path, extract_path);
                            if (er == 0) {
                                queue->items[i].state = DOWNLOAD_COMPLETED;
                                successful++;
                                total_files_extracted += extract_data.extracted_files;
                                download_success = true;
                            } else if (er == -2) {
                                cancelled = true;
                                break;
                            } else {
                                strncpy(queue->items[i].error_msg, "Extraction failed",
                                        sizeof(queue->items[i].error_msg) - 1);
                            }
                            remove(temp_path);
                        }
                    } else {
                        strncpy(queue->items[i].error_msg, "Download failed",
                                sizeof(queue->items[i].error_msg) - 1);
                    }

                    if (!download_success && queue->auto_retry && retries < queue->max_retries) {
                        retries++;
                        svcSleepThread(2000000000LL); // 2s before retry
                    } else {
                        break;
                    }
                } while (!download_success && retries <= queue->max_retries);

                if (!download_success && !cancelled) {
                    queue->items[i].state = DOWNLOAD_FAILED;
                    failed++;
                }
            }

            // Summary screen.
            char summary[160];
            snprintf(summary, sizeof(summary),
                     cancelled ? "Cancelled. OK %d, FAIL %d, SKIP %d, files %llu"
                               : "Done. OK %d, FAIL %d, SKIP %d, files %llu",
                     successful, failed, skipped,
                     (unsigned long long)total_files_extracted);
            gui_draw_status("Archive Extractor for 3DS", summary);

            started = false;
            while (aptMainLoop()) {
                hidScanInput();
                u32 kDown2 = hidKeysDown();
                if (kDown2 & KEY_START) goto exit_loop;
                if ((kDown2 & KEY_X) && failed > 0) {
                    show_queue = true;
                    queue_page = 0;
                    display_queue_status(queue, queue_page);
                    break;
                }
                if ((kDown2 & KEY_A) && failed > 0) {
                    for (int i = 0; i < url_count; i++) {
                        if (queue->items[i].state == DOWNLOAD_FAILED) {
                            queue->items[i].state = DOWNLOAD_PENDING;
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
    led_notification_off();
    restore_sleep_mode();

    download_cleanup_orphans();

    if (browser) free(browser);
    if (queue)   free(queue);

    curl_global_cleanup();
    socExit();
    free(socMemory);

    ptmuExit();
    gfxExit();
    return 0;
}
