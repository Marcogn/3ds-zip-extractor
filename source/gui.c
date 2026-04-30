#include "gui.h"
#include "gui_widgets.h"
#include "eta_format.h"

#include <stdio.h>
#include <string.h>

// Global render targets and text buffer (like fast-uninstall)
C3D_RenderTarget* g_top = NULL;
C3D_RenderTarget* g_bottom = NULL;
C2D_TextBuf g_textBuf = NULL;

// --- Bottom-screen persistent context ------------------------------------

static const DownloadQueue* s_bottom_queue   = NULL;
static const gui_scroll_t*  s_bottom_scroll  = NULL;
static int                  s_bottom_active  = -1;

void gui_set_bottom_context(const DownloadQueue* q, const gui_scroll_t* scroll) {
    s_bottom_queue  = q;
    s_bottom_scroll = scroll;
}

void gui_set_bottom_active(int idx) {
    s_bottom_active = idx;
}

// Forward declaration so the public wrapper can be defined here.
static void draw_bottom_queue_internal(void);

void gui_draw_bottom_persistent(void) {
    draw_bottom_queue_internal();
}

// --- Action bar layout shared by all bottom-screen states ----------------

#define ACTIONBAR_Y      198
#define ACTIONBAR_H       40
#define ACTIONBAR_BTNS     5

static gui_button_t s_action_buttons[ACTIONBAR_BTNS] = {
    { 4,   ACTIONBAR_Y, 58, ACTIONBAR_H, "A"     },
    { 66,  ACTIONBAR_Y, 58, ACTIONBAR_H, "B"     },
    { 128, ACTIONBAR_Y, 58, ACTIONBAR_H, "X"     },
    { 190, ACTIONBAR_Y, 58, ACTIONBAR_H, "Y"     },
    { 252, ACTIONBAR_Y, 64, ACTIONBAR_H, "START" },
};

// Codes returned through `out_action`. Order matches s_action_buttons.
static const char ACTION_CODES[ACTIONBAR_BTNS] = { 'A','B','X','Y','S' };

// --- Bottom queue rendering ----------------------------------------------

#define LIST_TOP        20
#define LIST_ROW_H      18
#define LIST_VISIBLE    9    // (198 - 20) / 18 = 9 rows

static u32 state_color(DownloadState s) {
    switch (s) {
        case DOWNLOAD_PENDING:     return COLOR_PENDING;
        case DOWNLOAD_IN_PROGRESS: return COLOR_PROGRESS;
        case DOWNLOAD_COMPLETED:   return COLOR_SUCCESS;
        case DOWNLOAD_FAILED:      return COLOR_ERROR;
        case DOWNLOAD_SKIPPED:     return COLOR_PANEL;
        default:                   return COLOR_TEXT;
    }
}

static const char* state_glyph(DownloadState s) {
    // Use ASCII glyphs — citro2d's default font has no Unicode support.
    switch (s) {
        case DOWNLOAD_PENDING:     return "o";
        case DOWNLOAD_IN_PROGRESS: return ">";
        case DOWNLOAD_COMPLETED:   return "v";
        case DOWNLOAD_FAILED:      return "x";
        case DOWNLOAD_SKIPPED:     return "-";
        default:                   return "?";
    }
}

static void draw_bottom_queue_internal(void) {
    C2D_TargetClear(g_bottom, COLOR_BG);
    C2D_SceneBegin(g_bottom);

    if (!s_bottom_queue) {
        // Just paint the action bar so the screen isn't empty.
        gui_widgets_draw_action_bar(s_action_buttons, ACTIONBAR_BTNS);
        return;
    }

    C2D_Text text;
    char buf[80];
    int total = s_bottom_queue->count;
    int offset = (s_bottom_scroll ? s_bottom_scroll->offset : 0);
    if (offset < 0) offset = 0;
    if (offset > total - LIST_VISIBLE && total > LIST_VISIBLE)
        offset = total - LIST_VISIBLE;
    if (total <= LIST_VISIBLE) offset = 0;

    // Header: title + count + scroll hint.
    snprintf(buf, sizeof(buf), "Queue (%d/%d)", total, MAX_URLS);
    C2D_TextParse(&text, g_textBuf, buf);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 6.0f, 2.0f, 0.5f, 0.4f, 0.4f, COLOR_ACCENT);

    if (total > LIST_VISIBLE) {
        const char* hint = "^v";
        C2D_TextParse(&text, g_textBuf, hint);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 290.0f, 2.0f, 0.5f, 0.4f, 0.4f, COLOR_PENDING);
    }

    // Separator.
    C2D_DrawRectSolid(0, 18, 0.5f, 320, 1, COLOR_ACCENT);

    int end = offset + LIST_VISIBLE;
    if (end > total) end = total;
    float y = LIST_TOP;
    for (int i = offset; i < end; i++) {
        const DownloadItem* it = &s_bottom_queue->items[i];

        // Row background highlight when this is the active item.
        if (i == s_bottom_active) {
            C2D_DrawRectSolid(0, y, 0.5f, 320, LIST_ROW_H, COLOR_PANEL);
        }

        // Status glyph.
        snprintf(buf, sizeof(buf), "%s", state_glyph(it->state));
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 6.0f, y + 2.0f, 0.55f,
                     0.5f, 0.5f, state_color(it->state));

        // URL (truncated).
        char short_url[42];
        snprintf(short_url, sizeof(short_url), "%.40s", it->url);
        C2D_TextParse(&text, g_textBuf, short_url);
        C2D_TextOptimize(&text);
        u32 fg = (it->state == DOWNLOAD_SKIPPED) ? COLOR_PENDING : COLOR_TEXT;
        C2D_DrawText(&text, C2D_WithColor, 22.0f, y + 3.0f, 0.55f,
                     0.38f, 0.38f, fg);
        y += LIST_ROW_H;
    }

    // Action bar.
    C2D_DrawRectSolid(0, ACTIONBAR_Y - 2, 0.5f, 320, 2, COLOR_ACCENT);
    gui_widgets_draw_action_bar(s_action_buttons, ACTIONBAR_BTNS);
}

void gui_draw_bottom_queue(const DownloadQueue* q,
                           const gui_scroll_t* scroll,
                           int active_idx) {
    const DownloadQueue* prev_q = s_bottom_queue;
    const gui_scroll_t*  prev_s = s_bottom_scroll;
    int                  prev_a = s_bottom_active;
    s_bottom_queue  = q;
    s_bottom_scroll = scroll;
    s_bottom_active = active_idx;
    draw_bottom_queue_internal();
    s_bottom_queue  = prev_q;
    s_bottom_scroll = prev_s;
    s_bottom_active = prev_a;
}

int gui_bottom_queue_handle(DownloadQueue* q,
                            gui_scroll_t* scroll,
                            bool interactive,
                            char* out_action) {
    if (out_action) *out_action = '\0';
    if (!q) return -1;

    int total = q->count;
    if (scroll) gui_scroll_update(scroll, total, LIST_VISIBLE);

    u32 kDown = hidKeysDown();
    if (!(kDown & KEY_TOUCH)) return -1;

    touchPosition tp;
    hidTouchRead(&tp);

    // Action-bar buttons first.
    for (int i = 0; i < ACTIONBAR_BTNS; i++) {
        if (gui_button_hit(&s_action_buttons[i], &tp)) {
            if (out_action) *out_action = ACTION_CODES[i];
            return -1;
        }
    }

    // List rows.
    if (!interactive) return -1;
    if ((int)tp.py < LIST_TOP) return -1;
    if ((int)tp.py >= ACTIONBAR_Y - 2) return -1;

    int row = ((int)tp.py - LIST_TOP) / LIST_ROW_H;
    int offset = scroll ? scroll->offset : 0;
    int idx = offset + row;
    if (idx < 0 || idx >= total) return -1;

    DownloadItem* it = &q->items[idx];
    if (it->state == DOWNLOAD_PENDING) {
        it->state = DOWNLOAD_SKIPPED;
        return idx;
    }
    if (it->state == DOWNLOAD_SKIPPED) {
        it->state = DOWNLOAD_PENDING;
        return idx;
    }
    // IN_PROGRESS / COMPLETED / FAILED → no toggle (per spec).
    return -1;
}

// --- Common rendering helpers --------------------------------------------

static void draw_top_title(const char* title) {
    C2D_DrawRectSolid(0, 0, 0.5f, 400, 22, COLOR_ACCENT);
    C2D_Text text;
    C2D_TextParse(&text, g_textBuf, title);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 5.0f, 3.0f, 0.5f, 0.55f, 0.55f,
                 C2D_Color32(0, 0, 0, 255));
}

static void draw_progress_bar(float x, float y, float w, float h,
                              float ratio) {
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    C2D_DrawRectSolid(x, y, 0.5f, w, h, COLOR_PANEL);
    C2D_DrawRectSolid(x + 2, y + 2, 0.6f, (w - 4) * ratio, h - 4, COLOR_PROGRESS);
    C2D_DrawRectSolid(x, y, 0.7f, w, 2, COLOR_ACCENT);
    C2D_DrawRectSolid(x, y + h - 2, 0.7f, w, 2, COLOR_ACCENT);
}

// --- Public draw functions -----------------------------------------------

bool gui_init(GUI* gui) {
    if (!gui) return false;
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    g_top    = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    g_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    g_textBuf = C2D_TextBufNew(4096);
    if (!g_top || !g_bottom || !g_textBuf) {
        if (g_textBuf) C2D_TextBufDelete(g_textBuf);
        C2D_Fini();
        C3D_Fini();
        return false;
    }
    gui->initialized = true;
    return true;
}

void gui_cleanup(GUI* gui) {
    if (g_textBuf) {
        C2D_TextBufDelete(g_textBuf);
        g_textBuf = NULL;
    }
    C2D_Fini();
    C3D_Fini();
    if (gui) gui->initialized = false;
}

void gui_draw_status(const char* title, const char* message) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);
    C2D_TextBufClear(g_textBuf);

    C2D_Text text;
    C2D_TextParse(&text, g_textBuf, title);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, 10.0f, 0.5f, 0.6f, 0.6f, COLOR_ACCENT);
    C2D_DrawRectSolid(10, 35, 0.5f, 380, 2, COLOR_ACCENT);
    if (message) {
        C2D_TextParse(&text, g_textBuf, message);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, 50.0f, 0.5f, 0.45f, 0.45f, COLOR_TEXT);
    }

    draw_bottom_queue_internal();
    C3D_FrameEnd(0);
}

void gui_draw_main_menu(int url_count, const char* config_path,
                        const char* extract_path, bool auto_retry,
                        int max_retries) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);
    C2D_TextBufClear(g_textBuf);

    draw_top_title(" Archive Extractor for 3DS");

    C2D_Text text;
    char buf[128];
    float y = 28.0f;

    if (url_count > 0) {
        snprintf(buf, sizeof(buf), "Loaded %d URL(s) from config", url_count);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.4f, 0.4f, COLOR_SUCCESS);
        y += 18;
        snprintf(buf, sizeof(buf), "Config: %s", config_path);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
        y += 14;
        snprintf(buf, sizeof(buf), "Extract to: %s", extract_path);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
        y += 14;
        if (auto_retry) {
            snprintf(buf, sizeof(buf), "Auto-retry: ON (max %d)", max_retries);
            C2D_TextParse(&text, g_textBuf, buf);
            C2D_TextOptimize(&text);
            C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
            y += 14;
        }
        y += 10;
        C2D_TextParse(&text, g_textBuf, "Supports: ZIP, TAR, 7Z, RAR, GZ, BZ2, XZ");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_TEXT);
        y += 20;
        C2D_DrawRectSolid(10, y, 0.5f, 380, 1, COLOR_ACCENT);
        y += 8;
        C2D_TextParse(&text, g_textBuf, "A: Start downloads");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_PROGRESS);
        y += 14;
        C2D_TextParse(&text, g_textBuf, "X: View queue        Y: Add URL (swkbd)");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
        y += 14;
        C2D_TextParse(&text, g_textBuf, "SELECT: Browse path  START: Exit");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
    } else {
        C2D_TextParse(&text, g_textBuf, "No URLs in config file!");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.45f, 0.45f, COLOR_ERROR);
        y += 20;
        C2D_TextParse(&text, g_textBuf, "Edit your config file:");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
        y += 14;
        snprintf(buf, sizeof(buf), "  %s", config_path);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
        y += 20;
        C2D_TextParse(&text, g_textBuf, "Y: Add URL via swkbd");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_PROGRESS);
        y += 16;
        C2D_TextParse(&text, g_textBuf, "START: Exit");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
    }

    draw_bottom_queue_internal();
    C3D_FrameEnd(0);
}

void gui_draw_download(int current, int total, const char* url,
                       u64 downloaded, u64 total_size,
                       double bps, int eta_seconds) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);
    C2D_TextBufClear(g_textBuf);

    C2D_Text text;
    float y = 10.0f;
    char buf[128];

    C2D_TextParse(&text, g_textBuf, "Archive Extractor for 3DS");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.5f, 0.5f, COLOR_ACCENT);
    y += 25;
    C2D_DrawRectSolid(10, y, 0.5f, 380, 2, COLOR_ACCENT);
    y += 12;

    if (total > 1) {
        snprintf(buf, sizeof(buf), "Downloading file %d of %d", current, total);
    } else {
        snprintf(buf, sizeof(buf), "Downloading...");
    }
    C2D_TextParse(&text, g_textBuf, buf);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.45f, 0.45f, COLOR_TEXT);
    y += 22;

    char urlShort[55];
    snprintf(urlShort, sizeof(urlShort), "%.52s", url ? url : "");
    C2D_TextParse(&text, g_textBuf, urlShort);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
    y += 20;

    if (total_size > 0) {
        double dl_mb  = (double)downloaded / (1024.0 * 1024.0);
        double tot_mb = (double)total_size / (1024.0 * 1024.0);
        float ratio = (float)((double)downloaded / (double)total_size);
        snprintf(buf, sizeof(buf), "%.2f MB / %.2f MB  (%.1f%%)",
                 dl_mb, tot_mb, ratio * 100.0f);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.42f, 0.42f, COLOR_TEXT);
        y += 25;
        draw_progress_bar(10, y, 380, 25, ratio);
        y += 32;
    } else {
        // Indeterminate.
        snprintf(buf, sizeof(buf), "%.2f MB",
                 (double)downloaded / (1024.0 * 1024.0));
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.42f, 0.42f, COLOR_TEXT);
        y += 25;
    }

    // Speed + ETA line.
    char eta[8];
    format_eta(eta_seconds, eta, sizeof(eta));
    if (bps > 0.0) {
        double mbps = bps / (1024.0 * 1024.0);
        snprintf(buf, sizeof(buf), "%.2f MB/s   ETA %s", mbps, eta);
    } else {
        snprintf(buf, sizeof(buf), "-- MB/s   ETA --:--");
    }
    C2D_TextParse(&text, g_textBuf, buf);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.4f, 0.4f, COLOR_TEXT);
    y += 22;

    C2D_TextParse(&text, g_textBuf, "Press B to cancel");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_PROGRESS);

    draw_bottom_queue_internal();
    C3D_FrameEnd(0);
}

void gui_draw_extraction(const char* filename, int files_done, int files_total) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);
    C2D_TextBufClear(g_textBuf);

    C2D_Text text;
    float y = 10.0f;
    char buf[80];

    C2D_TextParse(&text, g_textBuf, "Archive Extractor for 3DS");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.5f, 0.5f, COLOR_ACCENT);
    y += 25;
    C2D_DrawRectSolid(10, y, 0.5f, 380, 2, COLOR_ACCENT);
    y += 12;

    C2D_TextParse(&text, g_textBuf, "Extracting archive...");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.45f, 0.45f, COLOR_TEXT);
    y += 22;

    if (filename) {
        snprintf(buf, sizeof(buf), "%.55s", filename);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
        y += 20;
    }

    if (files_total > 0) {
        snprintf(buf, sizeof(buf), "Files: %d / %d", files_done, files_total);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.4f, 0.4f, COLOR_SUCCESS);
        y += 22;
        float ratio = (float)files_done / (float)files_total;
        draw_progress_bar(10, y, 380, 25, ratio);
        y += 30;
        snprintf(buf, sizeof(buf), "%.1f%%", ratio * 100.0f);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.4f, 0.4f, COLOR_TEXT);
        y += 22;
    } else {
        // Spinner fallback when total unknown.
        static const char SPIN[4] = {'|','/','-','\\'};
        unsigned long phase = (unsigned long)(osGetTime() / 100) & 3u;
        snprintf(buf, sizeof(buf), "Files extracted: %d  [%c]",
                 files_done, SPIN[phase]);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.4f, 0.4f, COLOR_SUCCESS);
        y += 25;
    }

    C2D_TextParse(&text, g_textBuf, "Press B to cancel");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_PROGRESS);

    draw_bottom_queue_internal();
    C3D_FrameEnd(0);
}

void gui_draw_error(const char* title, const char* message) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);
    C2D_TextBufClear(g_textBuf);

    C2D_Text text;
    C2D_TextParse(&text, g_textBuf, title);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, 10.0f, 0.5f, 0.5f, 0.5f, COLOR_ERROR);
    C2D_DrawRectSolid(10, 35, 0.5f, 380, 2, COLOR_ERROR);
    if (message) {
        C2D_TextParse(&text, g_textBuf, message);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, 50.0f, 0.5f, 0.4f, 0.4f, COLOR_TEXT);
    }
    C2D_TextParse(&text, g_textBuf, "Press START to exit");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, 200.0f, 0.5f, 0.38f, 0.38f, COLOR_PROGRESS);

    draw_bottom_queue_internal();
    C3D_FrameEnd(0);
}

void gui_draw_error_timed(const char* title, const char* message, int ms) {
    u64 deadline = osGetTime() + (u64)(ms > 0 ? ms : 0);
    while (aptMainLoop()) {
        gui_draw_error(title, message);
        hidScanInput();
        if (osGetTime() >= deadline) break;
        if (hidKeysDown() & (KEY_A | KEY_B | KEY_START)) break;
    }
}

int gui_confirm_prompt(const char* title, const char* message) {
    while (aptMainLoop()) {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(g_top, COLOR_BG);
        C2D_SceneBegin(g_top);
        C2D_TextBufClear(g_textBuf);

        C2D_Text text;
        C2D_TextParse(&text, g_textBuf, title ? title : "");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, 10.0f, 0.5f, 0.55f, 0.55f, COLOR_ACCENT);
        C2D_DrawRectSolid(10, 35, 0.5f, 380, 2, COLOR_ACCENT);
        if (message) {
            C2D_TextParse(&text, g_textBuf, message);
            C2D_TextOptimize(&text);
            C2D_DrawText(&text, C2D_WithColor, 10.0f, 50.0f, 0.5f, 0.42f, 0.42f, COLOR_TEXT);
        }
        C2D_TextParse(&text, g_textBuf, "A=yes   B=no");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, 200.0f, 0.5f, 0.42f, 0.42f, COLOR_PROGRESS);

        draw_bottom_queue_internal();
        C3D_FrameEnd(0);

        hidScanInput();
        u32 k = hidKeysDown();
        if (k & KEY_A) return 1;
        if (k & KEY_B) return 0;
        if (k & KEY_START) return -1;
    }
    return -1;
}

void gui_tooltip(const char* message, int ms) {
    u64 deadline = osGetTime() + (u64)(ms > 0 ? ms : 0);
    while (aptMainLoop()) {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(g_top, COLOR_BG);
        C2D_SceneBegin(g_top);
        C2D_TextBufClear(g_textBuf);

        C2D_Text text;
        C2D_TextParse(&text, g_textBuf, message ? message : "");
        C2D_TextOptimize(&text);
        // Centered-ish tooltip box.
        C2D_DrawRectSolid(40, 100, 0.5f, 320, 40, COLOR_PANEL);
        C2D_DrawRectSolid(40, 100, 0.55f, 320, 2, COLOR_ACCENT);
        C2D_DrawRectSolid(40, 138, 0.55f, 320, 2, COLOR_ACCENT);
        C2D_DrawText(&text, C2D_WithColor, 50.0f, 112.0f, 0.6f, 0.45f, 0.45f, COLOR_TEXT);

        draw_bottom_queue_internal();
        C3D_FrameEnd(0);

        hidScanInput();
        if (osGetTime() >= deadline) break;
    }
}

bool gui_confirm_persist_url(const char* url, int ms) {
    u64 deadline = osGetTime() + (u64)(ms > 0 ? ms : 0);
    bool persisted = false;
    while (aptMainLoop()) {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(g_top, COLOR_BG);
        C2D_SceneBegin(g_top);
        C2D_TextBufClear(g_textBuf);

        C2D_Text text;
        C2D_TextParse(&text, g_textBuf, "URL added to session");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, 10.0f, 0.5f, 0.55f, 0.55f, COLOR_SUCCESS);
        C2D_DrawRectSolid(10, 35, 0.5f, 380, 2, COLOR_SUCCESS);
        if (url) {
            char short_url[80];
            snprintf(short_url, sizeof(short_url), "%.76s", url);
            C2D_TextParse(&text, g_textBuf, short_url);
            C2D_TextOptimize(&text);
            C2D_DrawText(&text, C2D_WithColor, 10.0f, 50.0f, 0.5f, 0.4f, 0.4f, COLOR_TEXT);
        }
        C2D_TextParse(&text, g_textBuf, "Press R+A to also save it in config.txt");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, 90.0f, 0.5f, 0.4f, 0.4f, COLOR_PROGRESS);

        u64 now = osGetTime();
        u64 remaining = (now < deadline) ? (deadline - now) : 0;
        char buf[40];
        snprintf(buf, sizeof(buf), "(closing in %llus)",
                 (unsigned long long)((remaining + 999) / 1000));
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, 110.0f, 0.5f, 0.35f, 0.35f, COLOR_PENDING);

        draw_bottom_queue_internal();
        C3D_FrameEnd(0);

        hidScanInput();
        u32 held = hidKeysHeld();
        u32 down = hidKeysDown();
        if ((held & KEY_R) && (down & KEY_A)) {
            persisted = true;
            break;
        }
        if (down & (KEY_B | KEY_START)) break;
        if (now >= deadline) break;
    }
    return persisted;
}
