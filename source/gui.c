#include "gui.h"
#include <stdio.h>
#include <string.h>

// Initialize the hybrid GUI system
bool gui_init(GUI* gui) {
    if (!gui) return false;
    
    // Initialize citro3d
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    
    // Initialize citro2d
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    
    // Create render targets
    gui->top_screen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    gui->bottom_screen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    
    if (!gui->top_screen || !gui->bottom_screen) {
        gui_cleanup(gui);
        return false;
    }
    
    gui->initialized = true;
    return true;
}

// Cleanup GUI resources
void gui_cleanup(GUI* gui) {
    if (!gui || !gui->initialized) return;
    
    C2D_Fini();
    C3D_Fini();
    
    gui->initialized = false;
}

// Begin rendering frame
void gui_begin_frame(GUI* gui) {
    if (!gui || !gui->initialized) return;
    
    C3D_FrameBegin(C3D_FRAME_SYNCH);
}

// End rendering frame and display
void gui_end_frame(GUI* gui) {
    if (!gui || !gui->initialized) return;
    
    C3D_FrameEnd(0);
}

// Draw a filled rectangle (for progress bars)
static void draw_rectangle(float x, float y, float width, float height, u32 color) {
    C2D_DrawRectSolid(x, y, 0.5f, width, height, color);
}

// Draw a progress bar on screen
void gui_draw_progress_bar(float x, float y, float width, float height, 
                           float progress, u32 color_fill, u32 color_bg) {
    // Clamp progress between 0 and 1
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;
    
    // Draw background
    draw_rectangle(x, y, width, height, color_bg);
    
    // Draw fill
    if (progress > 0.0f) {
        float fill_width = width * progress;
        draw_rectangle(x, y, fill_width, height, color_fill);
    }
    
    // Draw border
    C2D_DrawRectSolid(x, y, 0.5f, width, 2, COLOR_TEXT);
    C2D_DrawRectSolid(x, y + height - 2, 0.5f, width, 2, COLOR_TEXT);
    C2D_DrawRectSolid(x, y, 0.5f, 2, height, COLOR_TEXT);
    C2D_DrawRectSolid(x + width - 2, y, 0.5f, 2, height, COLOR_TEXT);
}

// Draw download progress overlay on bottom screen
void gui_draw_download_progress(float progress, u64 downloaded, u64 total) {
    // Draw progress bar
    float bar_x = 20.0f;
    float bar_y = 180.0f;
    float bar_width = 280.0f;
    float bar_height = 30.0f;
    
    gui_draw_progress_bar(bar_x, bar_y, bar_width, bar_height, 
                         progress, COLOR_PROGRESS, COLOR_PENDING);
    
    // Draw percentage text in the center of the bar
    char percent_text[32];
    snprintf(percent_text, sizeof(percent_text), "%.1f%%", progress * 100.0f);
    
    // Note: citro2d doesn't have built-in text rendering
    // We'll use console for text, graphics for bars
}

// Draw extraction progress overlay on bottom screen
void gui_draw_extraction_progress(u64 files_extracted, const char* current_file) {
    // Draw progress indicator
    float bar_x = 20.0f;
    float bar_y = 180.0f;
    float bar_width = 280.0f;
    float bar_height = 30.0f;
    
    // For extraction, we don't know total, so show an animated bar
    static float anim_progress = 0.0f;
    anim_progress += 0.02f;
    if (anim_progress > 1.0f) anim_progress = 0.0f;
    
    gui_draw_progress_bar(bar_x, bar_y, bar_width, bar_height, 
                         anim_progress, COLOR_SUCCESS, COLOR_PENDING);
}
