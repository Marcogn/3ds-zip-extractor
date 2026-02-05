#include "gui.h"
#include <stdio.h>
#include <string.h>

// Initialize the hybrid GUI system
bool gui_init(GUI* gui) {
    if (!gui) return false;
    
    // GUI disabled for stability on real hardware
    // Console-only mode is used instead
    gui->initialized = false;
    return false;
}

// Cleanup GUI resources
void gui_cleanup(GUI* gui) {
    // Nothing to cleanup - GUI disabled
    if (!gui) return;
    gui->initialized = false;
}

// Begin rendering frame
void gui_begin_frame(GUI* gui) {
    // No-op - GUI disabled
}

// End rendering frame and display
void gui_end_frame(GUI* gui) {
    // No-op - GUI disabled
}

// Draw a progress bar on screen
void gui_draw_progress_bar(float x, float y, float width, float height, 
                           float progress, u32 color_fill, u32 color_bg) {
    // No-op - GUI disabled, console mode used
}

// Draw download progress overlay on bottom screen
void gui_draw_download_progress(float progress, u64 downloaded, u64 total) {
    // No-op - GUI disabled, console mode used
}

// Draw extraction progress overlay on bottom screen
void gui_draw_extraction_progress(u64 files_extracted, const char* current_file) {
    // No-op - GUI disabled, console mode used
}
