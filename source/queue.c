#include "queue.h"

#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include <citro2d.h>

#include "gui.h"

// Render the queue status screen. Called every frame while the queue view
// is active. Mirrors the original implementation that lived in main.c.
void display_queue_status(DownloadQueue* queue, int current_page) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C2D_TargetClear(g_top, COLOR_BG);
    C2D_SceneBegin(g_top);

    C2D_TextBufClear(g_textBuf);
    C2D_Text text;
    float y = 5.0f;

    // Title bar
    C2D_DrawRectSolid(0, 0, 0.5f, 400, 22, COLOR_ACCENT);
    C2D_TextParse(&text, g_textBuf, " Queue Status");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 5.0f, 3.0f, 0.5f, 0.5f, 0.5f, C2D_Color32(0, 0, 0, 255));
    y = 25.0f;

    int items_per_page = 10;
    int start = current_page * items_per_page;
    int end = start + items_per_page;
    if (end > queue->count) end = queue->count;

    char buf[64];
    snprintf(buf, sizeof(buf), "Showing %d-%d of %d", start + 1, end, queue->count);
    C2D_TextParse(&text, g_textBuf, buf);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PENDING);
    y += 15.0f;

    for (int i = start; i < end; i++) {
        const char* state_str = "?";
        u32 state_color = COLOR_TEXT;

        switch (queue->items[i].state) {
            case DOWNLOAD_PENDING:     state_str = "[ ]";  state_color = COLOR_PENDING;  break;
            case DOWNLOAD_IN_PROGRESS: state_str = "[>]";  state_color = COLOR_PROGRESS; break;
            case DOWNLOAD_COMPLETED:   state_str = "[OK]"; state_color = COLOR_SUCCESS;  break;
            case DOWNLOAD_FAILED:      state_str = "[X]";  state_color = COLOR_ERROR;    break;
            case DOWNLOAD_SKIPPED:     state_str = "[-]";  state_color = COLOR_PENDING;  break;
        }

        snprintf(buf, sizeof(buf), "%s %.42s", state_str, queue->items[i].url);
        C2D_TextParse(&text, g_textBuf, buf);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.33f, 0.33f, state_color);
        y += 13.0f;
    }

    int total_pages = (queue->count + items_per_page - 1) / items_per_page;
    if (total_pages == 0) total_pages = 1;

    y = 195.0f;
    snprintf(buf, sizeof(buf), "Page %d/%d", current_page + 1, total_pages);
    C2D_TextParse(&text, g_textBuf, buf);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.38f, 0.38f, COLOR_TEXT);
    y += 14.0f;

    C2D_TextParse(&text, g_textBuf, "L/R: Page  Y: Skip failed  B: Back");
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, C2D_WithColor, 10.0f, y, 0.5f, 0.35f, 0.35f, COLOR_PROGRESS);

    C2D_TargetClear(g_bottom, COLOR_BG);
    C2D_SceneBegin(g_bottom);

    C3D_FrameEnd(0);
}
