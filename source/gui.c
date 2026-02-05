#include "gui.h"
#include <stdio.h>
#include <string.h>
// Initialize GUI (console-only mode for stability)
bool gui_init(GUI* gui) {
    if (!gui) return false;
    // Console mode only - GPU rendering disabled for stability
    gui->initialized = false;
    return false; // Always return false to force console mode
}
// Cleanup GUI resources
void gui_cleanup(GUI* gui) {
    if (!gui) return;
    gui->initialized = false;
}
// Begin rendering frame (no-op in console mode)
void gui_begin_frame(GUI* gui) {
    // Console mode - no GPU rendering
}
// End rendering frame (no-op in console mode)
void gui_end_frame(GUI* gui) {
    // Console mode - no GPU rendering
}
// Draw progress bar (no-op in console mode)
void gui_draw_progress_bar(float x, float y, float width, float height, 
                           float progress, u32 color_fill, u32 color_bg) {
    // Console mode - progress shown via printf
}
// Draw download progress (no-op in console mode)
void gui_draw_download_progress(float progress, u64 downloaded, u64 total) {
    // Console mode - progress shown via printf
}
// Draw extraction progress (no-op in console mode)
void gui_draw_extraction_progress(u64 files_extracted, const char* current_file) {
    // Console mode - progress shown via printf
}
