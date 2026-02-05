#ifndef GUI_H
#define GUI_H

#include <3ds.h>

// GUI disabled for stability on real hardware
// Console-only mode is used instead

// Dummy colors for compatibility (not used)
#define COLOR_BG        0
#define COLOR_TEXT      0
#define COLOR_ACCENT    0
#define COLOR_SUCCESS   0
#define COLOR_ERROR     0
#define COLOR_PENDING   0
#define COLOR_PROGRESS  0

// GUI state (unused - kept for compatibility)
typedef struct {
    void* top_screen;
    void* bottom_screen;
    bool initialized;
} GUI;

// Initialize the hybrid GUI system
bool gui_init(GUI* gui);

// Cleanup GUI resources
void gui_cleanup(GUI* gui);

// Draw a progress bar on screen
void gui_draw_progress_bar(float x, float y, float width, float height, 
                           float progress, u32 color_fill, u32 color_bg);

// Draw text overlay on console
void gui_draw_text_overlay(const char* text, float x, float y, u32 color);

// Begin rendering frame
void gui_begin_frame(GUI* gui);

// End rendering frame and display
void gui_end_frame(GUI* gui);

// Draw download progress overlay
void gui_draw_download_progress(float progress, u64 downloaded, u64 total);

// Draw extraction progress overlay
void gui_draw_extraction_progress(u64 files_extracted, const char* current_file);

#endif // GUI_H
