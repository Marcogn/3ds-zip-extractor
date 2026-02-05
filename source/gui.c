#include "gui.h"
#include <stdio.h>
#include <string.h>
// Global render targets and text buffer (like fast-uninstall)
C3D_RenderTarget* g_top = NULL;
C3D_RenderTarget* g_bottom = NULL;
C2D_TextBuf g_textBuf = NULL;
// Initialize GUI (exactly like fast-uninstall)
bool gui_init(GUI* gui) {
    if (!gui) return false;
    // Initialize citro3d
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    // Initialize citro2d
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    // Create render targets for top and bottom screens
    g_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    g_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    // Create text buffer for dynamic text rendering
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
// Cleanup GUI resources
void gui_cleanup(GUI* gui) {
    if (g_textBuf) {
        C2D_TextBufDelete(g_textBuf);
        g_textBuf = NULL;
    }
    C2D_Fini();
    C3D_Fini();
    if (gui) gui->initialized = false;
}
// Draw a loading/status screen (like fast-uninstall's drawLoadingScreen)
void gui_draw_status(const char* title, const char* message) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    // TOP SCREEN
    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);
    C2D_TextBufClear(g_textBuf);
    C2D_Text text;
    // Title
    C2D_TextParse(&text, g_textBuf, title);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, 10.0f, 0.5f, 0.6f, 0.6f, COLOR_ACCENT);
    // Separator line
    C2D_DrawRectSolid(10, 35, 0.5f, 380, 2, COLOR_ACCENT);
    // Message
    if (message) {
        C2D_TextParse(&text, g_textBuf, message);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, 50.0f, 0.5f, 0.45f, 0.45f, COLOR_TEXT);
    }
    // BOTTOM SCREEN
    C2D_TargetClear(g_bottom, COLOR_BG);
    C2D_SceneBegin(g_bottom);
    C3D_FrameEnd(0);
}
// Draw main menu
void gui_draw_main_menu(int url_count, const char* config_path, const char* extract_path, bool auto_retry, int max_retries) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    // TOP SCREEN
    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);
    C2D_TextBufClear(g_textBuf);
    C2D_Text text;
    float y = 5.0f;
    // Title bar
    C2D_DrawRectSolid(0, 0, 0.5f, 400, 22, COLOR_ACCENT);
    C2D_TextParse(&text, g_textBuf, " Archive Extractor for 3DS");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 5.0f, 3.0f, 0.5f, 0.55f, 0.55f, C2D_Color32(0, 0, 0, 255));
    y = 28.0f;
    char buf[128];
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
        // Controls
        C2D_DrawRectSolid(10, y, 0.5f, 380, 1, COLOR_ACCENT);
        y += 8;
        C2D_TextParse(&text, g_textBuf, "A: Start downloads");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_PROGRESS);
        y += 14;
        C2D_TextParse(&text, g_textBuf, "X: View queue");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
        y += 14;
        C2D_TextParse(&text, g_textBuf, "SELECT: Browse path");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
        y += 14;
        C2D_TextParse(&text, g_textBuf, "START: Exit");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
    } else {
        C2D_TextParse(&text, g_textBuf, "No config file found!");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.45f, 0.45f, COLOR_ERROR);
        y += 20;
        C2D_TextParse(&text, g_textBuf, "Please create:");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
        y += 14;
        snprintf(buf, sizeof(buf), "  %s", config_path);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
        y += 20;
        C2D_TextParse(&text, g_textBuf, "Example config.txt:");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
        y += 14;
        C2D_TextParse(&text, g_textBuf, "  extract_path=/extracted/");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
        y += 12;
        C2D_TextParse(&text, g_textBuf, "  auto_retry=true");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
        y += 12;
        C2D_TextParse(&text, g_textBuf, "  https://example.com/file.zip");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
        y += 20;
        C2D_TextParse(&text, g_textBuf, "START: Exit");
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
    }
    // BOTTOM SCREEN
    C2D_TargetClear(g_bottom, COLOR_BG);
    C2D_SceneBegin(g_bottom);
    C3D_FrameEnd(0);
}
// Draw download progress
void gui_draw_download(int current, int total, const char* url, u64 downloaded, u64 total_size) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    // TOP SCREEN
    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);
    C2D_TextBufClear(g_textBuf);
    C2D_Text text;
    float y = 10.0f;
    // Title
    C2D_TextParse(&text, g_textBuf, "Archive Extractor for 3DS");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.5f, 0.5f, COLOR_ACCENT);
    y += 25;
    C2D_DrawRectSolid(10, y, 0.5f, 380, 2, COLOR_ACCENT);
    y += 12;
    // Status
    char buf[128];
    if (total > 1) {
        snprintf(buf, sizeof(buf), "Downloading file %d of %d", current, total);
    } else {
        snprintf(buf, sizeof(buf), "Downloading...");
    }
    C2D_TextParse(&text, g_textBuf, buf);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.45f, 0.45f, COLOR_TEXT);
    y += 22;
    // URL (truncated)
    char urlShort[55];
    snprintf(urlShort, sizeof(urlShort), "%.52s", url);
    C2D_TextParse(&text, g_textBuf, urlShort);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
    y += 20;
    // Progress info
    if (total_size > 0) {
        double dl_mb = (double)downloaded / (1024.0 * 1024.0);
        double tot_mb = (double)total_size / (1024.0 * 1024.0);
        float progress = (float)downloaded / (float)total_size;
        snprintf(buf, sizeof(buf), "%.2f MB / %.2f MB  (%.1f%%)", dl_mb, tot_mb, progress * 100.0f);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.42f, 0.42f, COLOR_TEXT);
        y += 25;
        // Progress bar background
        C2D_DrawRectSolid(10, y, 0.5f, 380, 25, COLOR_PANEL);
        // Progress bar fill
        C2D_DrawRectSolid(12, y + 2, 0.6f, 376 * progress, 21, COLOR_PROGRESS);
        // Progress bar border
        C2D_DrawRectSolid(10, y, 0.7f, 380, 2, COLOR_ACCENT);
        C2D_DrawRectSolid(10, y + 23, 0.7f, 380, 2, COLOR_ACCENT);
        y += 35;
    }
    // Controls
    C2D_TextParse(&text, g_textBuf, "Press B to cancel");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_PROGRESS);
    // BOTTOM SCREEN
    C2D_TargetClear(g_bottom, COLOR_BG);
    C2D_SceneBegin(g_bottom);
    C3D_FrameEnd(0);
}
// Draw extraction progress
void gui_draw_extraction(const char* filename, int files_done) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    // TOP SCREEN
    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);
    C2D_TextBufClear(g_textBuf);
    C2D_Text text;
    float y = 10.0f;
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
        char buf[64];
        snprintf(buf, sizeof(buf), "%.50s", filename);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
        y += 18;
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "Files extracted: %d", files_done);
    C2D_TextParse(&text, g_textBuf, buf);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.4f, 0.4f, COLOR_SUCCESS);
    y += 25;
    C2D_TextParse(&text, g_textBuf, "Press B to cancel");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_PROGRESS);
    // BOTTOM SCREEN
    C2D_TargetClear(g_bottom, COLOR_BG);
    C2D_SceneBegin(g_bottom);
    C3D_FrameEnd(0);
}
// Draw error screen
void gui_draw_error(const char* title, const char* message) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    // TOP SCREEN
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
    // BOTTOM SCREEN
    C2D_TargetClear(g_bottom, COLOR_BG);
    C2D_SceneBegin(g_bottom);
    C3D_FrameEnd(0);
}
