#include "file_browser.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include <citro2d.h>

#include "gui.h"

void init_file_browser(FileBrowser* browser, const char* start_path) {
    strncpy(browser->current_path, start_path, MAX_PATH_LENGTH - 1);
    browser->current_path[MAX_PATH_LENGTH - 1] = '\0';
    browser->count = 0;
    browser->selected = 0;
}

int load_directory(FileBrowser* browser) {
    DIR* dir = opendir(browser->current_path);
    if (!dir) {
        return -1;
    }

    browser->count = 0;

    // Add parent directory entry if not at root
    if (strcmp(browser->current_path, "sdmc:/") != 0) {
        strncpy(browser->entries[browser->count].name, "..", sizeof(browser->entries[0].name) - 1);
        browser->entries[browser->count].name[sizeof(browser->entries[0].name) - 1] = '\0';
        browser->entries[browser->count].is_directory = true;
        browser->count++;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && browser->count < MAX_DIR_ENTRIES) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        strncpy(browser->entries[browser->count].name, entry->d_name,
                sizeof(browser->entries[0].name) - 1);
        browser->entries[browser->count].name[sizeof(browser->entries[0].name) - 1] = '\0';
        browser->entries[browser->count].is_directory = (entry->d_type == DT_DIR);
        browser->count++;
    }
    closedir(dir);

    // Sort: directories first, then alphabetically.
    for (int i = 0; i < browser->count - 1; i++) {
        for (int j = i + 1; j < browser->count; j++) {
            bool swap = false;
            if (browser->entries[i].is_directory && !browser->entries[j].is_directory) {
                continue;
            }
            if (!browser->entries[i].is_directory && browser->entries[j].is_directory) {
                swap = true;
            } else if (strcmp(browser->entries[i].name, browser->entries[j].name) > 0) {
                swap = true;
            }
            if (swap) {
                DirEntry tmp = browser->entries[i];
                browser->entries[i] = browser->entries[j];
                browser->entries[j] = tmp;
            }
        }
    }

    browser->selected = 0;
    return browser->count;
}

void display_file_browser(FileBrowser* browser, int scroll_offset) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);

    C2D_TextBufClear(g_textBuf);
    C2D_Text text;
    float y = 5.0f;

    C2D_DrawRectSolid(0, 0, 0.5f, 400, 22, COLOR_ACCENT);
    C2D_TextParse(&text, g_textBuf, " File Browser - Select Path");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 5.0f, 3.0f, 0.5f, 0.5f, 0.5f, C2D_Color32(0, 0, 0, 255));
    y = 25.0f;

    char pathBuf[64];
    snprintf(pathBuf, sizeof(pathBuf), "%.50s", browser->current_path);
    C2D_TextParse(&text, g_textBuf, pathBuf);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
    y += 15.0f;

    C2D_DrawRectSolid(10, y, 0.5f, 380, 1, COLOR_ACCENT);
    y += 5.0f;

    int visible_lines = 10;
    int start = scroll_offset;
    int end = start + visible_lines;
    if (end > browser->count) end = browser->count;

    for (int i = start; i < end; i++) {
        char lineBuf[64];
        const char* type_marker = browser->entries[i].is_directory ? "/" : "";
        snprintf(lineBuf, sizeof(lineBuf), "%.45s%s", browser->entries[i].name, type_marker);

        if (i == browser->selected) {
            C2D_DrawRectSolid(10, y - 1, 0.5f, 380, 14, COLOR_ACCENT);
            C2D_TextParse(&text, g_textBuf, lineBuf);
            C2D_TextOptimize(&text);
            C2D_DrawText(&text, C2D_WithColor, 15.0f, y, 0.5f, 0.38f, 0.38f, C2D_Color32(0, 0, 0, 255));
        } else {
            C2D_TextParse(&text, g_textBuf, lineBuf);
            C2D_TextOptimize(&text);
            C2D_DrawText(&text, C2D_WithColor, 15.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
        }
        y += 14.0f;
    }

    y = 200.0f;
    C2D_TextParse(&text, g_textBuf, "A: Enter/Extract  Y: Use as path");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PROGRESS);
    y += 12;
    C2D_TextParse(&text, g_textBuf, "B: Back  X: Refresh");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_TEXT);

    C2D_TargetClear(g_bottom, COLOR_BG);
    C2D_SceneBegin(g_bottom);

    C3D_FrameEnd(0);
}
