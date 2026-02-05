#include "gui.h"
#include <stdio.h>
#include <string.h>

// Initialize the GUI system with citro3d/citro2d
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

    // Create render targets for both screens
    gui->top_screen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    gui->bottom_screen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    if (!gui->top_screen || !gui->bottom_screen) {
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

// Draw a filled rectangle
static void draw_rectangle(float x, float y, float width, float height, u32 color) {
    C2D_DrawRectSolid(x, y, 0.5f, width, height, color);
}

// Draw rectangle with border
static void draw_panel(float x, float y, float width, float height, u32 bg_color, u32 border_color) {
    // Background
    draw_rectangle(x, y, width, height, bg_color);

    // Border
    draw_rectangle(x, y, width, 2, border_color);                    // Top
    draw_rectangle(x, y + height - 2, width, 2, border_color);       // Bottom
    draw_rectangle(x, y, 2, height, border_color);                   // Left
    draw_rectangle(x + width - 2, y, 2, height, border_color);       // Right
}

// Draw a progress bar
void gui_draw_progress_bar(float x, float y, float width, float height,
                           float progress, u32 color_fill, u32 color_bg) {
    // Clamp progress
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    // Background panel
    draw_panel(x, y, width, height, color_bg, COLOR_ACCENT);

    // Progress fill
    if (progress > 0.0f) {
        float fill_width = (width - 8) * progress;
        draw_rectangle(x + 4, y + 4, fill_width, height - 8, color_fill);
    }
}

// Draw top screen with title and status
void gui_draw_top_screen(const char* title, const char* status, const char* info) {
    // Clear with dark blue background
    C2D_TargetClear(C3D_RenderTargetGetColorBuffer(NULL, GFX_TOP, GFX_LEFT), COLOR_BG);

    C2D_SceneBegin(NULL);

    // Title bar
    draw_panel(10, 10, 380, 40, COLOR_PANEL, COLOR_ACCENT);

    // Status panel
    draw_panel(10, 60, 380, 160, COLOR_PANEL, COLOR_ACCENT);

    // Note: Text rendering requires custom font setup
    // For now we use console overlay for text
}

// Draw bottom screen with controls
void gui_draw_bottom_screen(const char* controls) {
    // Clear with dark blue background
    C2D_TargetClear(C3D_RenderTargetGetColorBuffer(NULL, GFX_BOTTOM, GFX_LEFT), COLOR_BG);

    C2D_SceneBegin(NULL);

    // Control panel
    draw_panel(10, 10, 300, 220, COLOR_PANEL, COLOR_ACCENT);
}

// Draw download progress on bottom screen
void gui_draw_download_progress(float progress, u64 downloaded, u64 total) {
    // Progress bar
    float bar_x = 20.0f;
    float bar_y = 160.0f;
    float bar_width = 280.0f;
    float bar_height = 40.0f;

    gui_draw_progress_bar(bar_x, bar_y, bar_width, bar_height,
                         progress, COLOR_PROGRESS, COLOR_PENDING);
}

// Draw extraction progress on bottom screen
void gui_draw_extraction_progress(u64 files_extracted, const char* current_file) {
    // Animated progress bar
    static float anim_progress = 0.0f;
    anim_progress += 0.02f;
    if (anim_progress > 1.0f) anim_progress = 0.0f;

    float bar_x = 20.0f;
    float bar_y = 160.0f;
    float bar_width = 280.0f;
    float bar_height = 40.0f;

    gui_draw_progress_bar(bar_x, bar_y, bar_width, bar_height,
                         anim_progress, COLOR_SUCCESS, COLOR_PENDING);
}
