#ifndef GUI_H
#define GUI_H

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>

// Dark blue theme colors (similar to fast-uninstall style)
#define COLOR_BG        C2D_Color32(20, 20, 30, 255)
#define COLOR_PANEL     C2D_Color32(30, 30, 40, 255)
#define COLOR_TEXT      C2D_Color32(255, 255, 255, 255)
#define COLOR_ACCENT    C2D_Color32(100, 180, 255, 255)
#define COLOR_SUCCESS   C2D_Color32(76, 175, 80, 255)
#define COLOR_ERROR     C2D_Color32(244, 67, 54, 255)
#define COLOR_PENDING   C2D_Color32(150, 150, 150, 255)
#define COLOR_PROGRESS  C2D_Color32(255, 200, 100, 255)

// GUI state matching fast-uninstall structure
typedef struct {
    C3D_RenderTarget* top;
    C3D_RenderTarget* bottom;
    C2D_TextBuf textBuf;
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
void gui_draw_progress_bar(GUI* gui, float x, float y, float width, float height,
                           float progress, u32 color_fill, u32 color_bg);
void gui_draw_download_progress(float progress, u64 downloaded, u64 total);
void gui_draw_extraction_progress(u64 files_extracted, const char* current_file);

#endif // GUI_H
