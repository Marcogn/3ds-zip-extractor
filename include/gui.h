#ifndef GUI_H
#define GUI_H

#include <3ds.h>
#include <citro2d.h>
#include <stdbool.h>

#include "gui_widgets.h"
#include "queue.h"

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

// Draw download progress, with rolling speed (`bps`) and ETA
// (`eta_seconds`, < 0 if unknown). Pass `bps <= 0` to suppress the
// speed/ETA line.
void gui_draw_download(int current, int total, const char* url,
                       u64 downloaded, u64 total_size,
                       double bps, int eta_seconds);

// Draw extraction progress.
//   - `files_total > 0` → percentage progress bar (`files_done / files_total`)
//   - `files_total <= 0` → rotating spinner over `osGetTime()`
void gui_draw_extraction(const char* filename, int files_done, int files_total);

// Draw error screen
void gui_draw_error(const char* title, const char* message);

// Draw an error overlay that auto-dismisses after `ms` milliseconds.
// Cooperative: keeps `aptMainLoop()` happy.
void gui_draw_error_timed(const char* title, const char* message, int ms);

// Draw the persistent bottom-screen queue (a scrollable list of items
// plus a 5-button virtual action bar). The function only renders; it
// does not commit a frame on its own.
//
// Call between C2D_TargetClear(g_bottom, ...) / C2D_SceneBegin(g_bottom)
// blocks. `active_idx` highlights the currently-running item (use -1 if
// none). `interactive == false` disables touch toggle (READ_ONLY mode
// used while download is running).
void gui_draw_bottom_queue(const DownloadQueue* q,
                           const gui_scroll_t* scroll,
                           int active_idx);

// Process touch + key input on the bottom-screen queue.
// Returns the index of the item the user toggled (PENDING ↔ SKIPPED) or
// -1 if no toggle happened. `out_action`, when non-NULL, receives the
// virtual action-bar button activated this frame:
//   '\0' = none, 'A', 'B', 'X', 'Y', 'S' (=START).
// `interactive == false` disables list toggling.
int gui_bottom_queue_handle(DownloadQueue* q,
                            gui_scroll_t* scroll,
                            bool interactive,
                            char* out_action);

// Show "URL added — press R+A to save" banner for `ms` milliseconds.
// Returns true if the user pressed R+A within the timeout.
bool gui_confirm_persist_url(const char* url, int ms);

// Persistent bottom-screen context: when set, every top-screen render
// (`gui_draw_main_menu`, `gui_draw_download`, `gui_draw_extraction`)
// also paints the interactive bottom queue + action bar.
//
// `scroll` is a caller-owned scroll state, mutated by the input handler
// every frame. Pass NULL to clear the context.
void gui_set_bottom_context(const DownloadQueue* q, const gui_scroll_t* scroll);

// Mark `idx` as the currently-running item (highlighted in COLOR_PROGRESS).
// Pass `-1` when no item is running.
void gui_set_bottom_active(int idx);

// Generic two-button confirmation prompt drawn on the top screen.
// Returns 1 if user pressed A, 0 if B, -1 on START / exit.
int gui_confirm_prompt(const char* title, const char* message);

// Tooltip overlay shown for `ms` ms (cooperative). Useful for
// "Not a supported archive" and similar transient messages.
void gui_tooltip(const char* message, int ms);

// Render the persistent bottom screen using the context previously set
// via gui_set_bottom_context()/gui_set_bottom_active(). Intended to be
// called from inside an existing C3D_FrameBegin block, after the top
// scene has been drawn. Does NOT call C3D_FrameBegin/End itself.
void gui_draw_bottom_persistent(void);

#endif // GUI_H
