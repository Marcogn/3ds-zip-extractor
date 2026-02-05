#include "gui.h"
#include <stdio.h>
#include <string.h>
// Initialize GUI system (exactly like fast-uninstall)
bool gui_init(GUI* gui) {
    if (!gui) return false;
    // Initialize citro3d
    if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE)) {
        return false;
    }
    // Initialize citro2d
    if (!C2D_Init(C2D_DEFAULT_MAX_OBJECTS)) {
        C3D_Fini();
        return false;
    }
    C2D_Prepare();
    // Create render targets for top and bottom screens
    gui->top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    gui->bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    if (!gui->top || !gui->bottom) {
        C2D_Fini();
        C3D_Fini();
        return false;
    }
    // Create text buffer for dynamic text rendering
    gui->textBuf = C2D_TextBufNew(4096);
    if (!gui->textBuf) {
        C2D_Fini();
        C3D_Fini();
        return false;
    }
    gui->initialized = true;
    return true;
}
// Cleanup GUI resources
void gui_cleanup(GUI* gui) {
    if (!gui || !gui->initialized) return;
    if (gui->textBuf) {
        C2D_TextBufDelete(gui->textBuf);
    }
    C2D_Fini();
    C3D_Fini();
    gui->initialized = false;
}
// Begin rendering frame
void gui_begin_frame(GUI* gui) {
    if (!gui || !gui->initialized) return;
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
}
// End rendering frame and display
void gui_end_frame(GUI* gui) {
    if (!gui || !gui->initialized) return;
    C3D_FrameEnd(0);
}
// Draw a progress bar (simple rectangle-based like fast-uninstall)
void gui_draw_progress_bar(GUI* gui, float x, float y, float width, float height, 
                           float progress, u32 color_fill, u32 color_bg) {
    if (!gui || !gui->initialized) return;
    // Clamp progress
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;
    // Draw background
    C2D_DrawRectSolid(x, y, 0.5f, width, height, color_bg);
    // Draw progress fill
    if (progress > 0.0f) {
        float fill_width = width * progress;
        C2D_DrawRectSolid(x, y, 0.6f, fill_width, height, color_fill);
    }
    // Draw border
    C2D_DrawRectSolid(x, y, 0.7f, width, 1, COLOR_ACCENT);
    C2D_DrawRectSolid(x, y + height - 1, 0.7f, width, 1, COLOR_ACCENT);
    C2D_DrawRectSolid(x, y, 0.7f, 1, height, COLOR_ACCENT);
    C2D_DrawRectSolid(x + width - 1, y, 0.7f, 1, height, COLOR_ACCENT);
}
// Draw download progress (no-op for now, can be extended)
void gui_draw_download_progress(float progress, u64 downloaded, u64 total) {
    // Text drawing handled externally with C2D_TextParse
}
// Draw extraction progress (no-op for now, can be extended)
void gui_draw_extraction_progress(u64 files_extracted, const char* current_file) {
    // Text drawing handled externally with C2D_TextParse
}
