#ifndef GUI_H
#define GUI_H

#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>

// Dark blue theme colors
#define COLOR_BG        C2D_Color32(15, 20, 40, 255)      // Blu scuro per sfondo
#define COLOR_PANEL     C2D_Color32(25, 35, 60, 255)      // Pannelli
#define COLOR_TEXT      C2D_Color32(255, 255, 255, 255)   // Testo bianco
#define COLOR_ACCENT    C2D_Color32(60, 120, 240, 255)    // Blu chiaro accento
#define COLOR_SUCCESS   C2D_Color32(76, 175, 80, 255)     // Verde successo
#define COLOR_ERROR     C2D_Color32(244, 67, 54, 255)     // Rosso errore
#define COLOR_PENDING   C2D_Color32(120, 130, 150, 255)   // Grigio pending
#define COLOR_PROGRESS  C2D_Color32(100, 180, 255, 255)   // Blu progress

// GUI state
typedef struct {
    C3D_RenderTarget* top_screen;
    C3D_RenderTarget* bottom_screen;
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
void gui_draw_top_screen(const char* title, const char* status, const char* info);
void gui_draw_bottom_screen(const char* controls);
void gui_draw_download_progress(float progress, u64 downloaded, u64 total);
void gui_draw_extraction_progress(u64 files_extracted, const char* current_file);

#endif // GUI_H
