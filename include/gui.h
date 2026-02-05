#ifndef GUI_H
#define GUI_H

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>

// Dark blue theme colors (like fast-uninstall)
#define COLOR_BG        C2D_Color32(20, 20, 30, 255)
#define COLOR_PANEL     C2D_Color32(30, 30, 40, 255)
#define COLOR_TEXT      C2D_Color32(255, 255, 255, 255)
#define COLOR_ACCENT    C2D_Color32(100, 180, 255, 255)
#define COLOR_SUCCESS   C2D_Color32(76, 175, 80, 255)
#define COLOR_ERROR     C2D_Color32(244, 67, 54, 255)
#define COLOR_PENDING   C2D_Color32(200, 200, 200, 255)
#define COLOR_PROGRESS  C2D_Color32(255, 200, 100, 255)

// Global render targets and text buffer (like fast-uninstall)
extern C3D_RenderTarget* g_top;
extern C3D_RenderTarget* g_bottom;
extern C2D_TextBuf g_textBuf;

// GUI state
typedef struct {
    bool initialized;
} GUI;

// Initialize the GUI system
bool gui_init(GUI* gui);

// Cleanup GUI resources
void gui_cleanup(GUI* gui);

// Draw a loading/status screen (like fast-uninstall's drawLoadingScreen)
void gui_draw_status(const char* title, const char* message);

// Draw main menu
void gui_draw_main_menu(int url_count, const char* config_path, const char* extract_path, bool auto_retry, int max_retries);

// Draw download progress
void gui_draw_download(int current, int total, const char* url, u64 downloaded, u64 total_size);

// Draw extraction progress
void gui_draw_extraction(const char* filename, int files_done);

// Draw error screen
void gui_draw_error(const char* title, const char* message);

#endif // GUI_H
