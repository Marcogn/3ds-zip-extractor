#ifndef GUI_H
#define GUI_H

#include <citro2d.h>
#include <3ds.h>

// Colors for UI elements
#define COLOR_BG        C2D_Color32(30, 30, 40, 255)
#define COLOR_TEXT      C2D_Color32(255, 255, 255, 255)
#define COLOR_ACCENT    C2D_Color32(66, 135, 245, 255)
#define COLOR_SUCCESS   C2D_Color32(76, 175, 80, 255)
#define COLOR_ERROR     C2D_Color32(244, 67, 54, 255)
#define COLOR_PENDING   C2D_Color32(158, 158, 158, 255)
#define COLOR_PROGRESS  C2D_Color32(255, 193, 7, 255)

// GUI state
typedef struct {
    C3D_RenderTarget* top_screen;
    C3D_RenderTarget* bottom_screen;
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
