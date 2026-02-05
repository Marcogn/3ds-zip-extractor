#ifndef GUI_H
#define GUI_H

#include <3ds.h>
#include <stdbool.h>

// Console-only mode for maximum stability
// GPU rendering disabled

// Dummy colors (not used in console mode)
#define COLOR_BG        0
#define COLOR_PANEL     0
#define COLOR_TEXT      0
#define COLOR_ACCENT    0
#define COLOR_SUCCESS   0
#define COLOR_ERROR     0
#define COLOR_PENDING   0
#define COLOR_PROGRESS  0

// GUI state (minimal for compatibility)
typedef struct {
    bool initialized;
} GUI;

// Initialize the GUI system
bool gui_init(GUI* gui);

// Cleanup GUI resources
void gui_cleanup(GUI* gui);

// Frame management
void gui_begin_frame(GUI* gui);
void gui_end_frame(GUI* gui);

// Drawing functions
void gui_draw_progress_bar(float x, float y, float width, float height,
                           float progress, u32 color_fill, u32 color_bg);
void gui_draw_download_progress(float progress, u64 downloaded, u64 total);
void gui_draw_extraction_progress(u64 files_extracted, const char* current_file);

#endif // GUI_H
