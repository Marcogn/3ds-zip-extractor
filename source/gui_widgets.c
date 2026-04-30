#include "gui_widgets.h"

#ifndef HOST_BUILD

#include <stdio.h>
#include <string.h>
#include <citro2d.h>

#include "gui.h"

bool gui_button_hit(const gui_button_t* btn, const touchPosition* tp) {
    if (!btn || !tp) return false;
    float fx = (float)tp->px;
    float fy = (float)tp->py;
    return (fx >= btn->x) && (fx < btn->x + btn->w) &&
           (fy >= btn->y) && (fy < btn->y + btn->h);
}

void gui_widgets_draw_button(const gui_button_t* btn, bool pressed) {
    if (!btn) return;

    u32 fill = pressed ? COLOR_ACCENT : COLOR_PANEL;
    u32 fg   = pressed ? C2D_Color32(0, 0, 0, 255) : COLOR_TEXT;

    // Border
    C2D_DrawRectSolid(btn->x, btn->y, 0.5f, btn->w, btn->h, COLOR_ACCENT);
    // Fill (1 px inset)
    C2D_DrawRectSolid(btn->x + 1, btn->y + 1, 0.55f,
                      btn->w - 2, btn->h - 2, fill);

    if (btn->label) {
        C2D_Text text;
        C2D_TextParse(&text, g_textBuf, btn->label);
        C2D_TextOptimize(&text);

        // Rough centering: 0.45 scale ~ 7 px per char, height ~ 11 px.
        float scale = 0.45f;
        float tw, th;
        C2D_TextGetDimensions(&text, scale, scale, &tw, &th);
        float tx = btn->x + (btn->w - tw) * 0.5f;
        float ty = btn->y + (btn->h - th) * 0.5f;
        C2D_DrawText(&text, C2D_WithColor, tx, ty, 0.6f, scale, scale, fg);
    }
}

void gui_widgets_draw_action_bar(const gui_button_t* buttons, int n) {
    if (!buttons || n <= 0) return;
    for (int i = 0; i < n; i++) {
        gui_widgets_draw_button(&buttons[i], false);
    }
}

void gui_scroll_update(gui_scroll_t* s, int item_count, int visible_rows) {
    if (!s) return;
    if (item_count <= visible_rows) {
        s->offset = 0;
        s->dragging = false;
        return;
    }

    u32 kHeld = hidKeysHeld();
    u32 kDown = hidKeysDown();
    u32 kUp   = hidKeysUp();

    // D-Pad / circle pad scroll (one step per frame held is too fast;
    // gate on kDown for keyboardy step, kHeld for slow drag-like accel).
    if (kDown & (KEY_DUP | KEY_CPAD_UP))   s->offset--;
    if (kDown & (KEY_DDOWN | KEY_CPAD_DOWN)) s->offset++;

    // Touch drag: while finger is held, accumulate delta in pixels and
    // turn it into row-steps every 18 px (one row).
    if (kDown & KEY_TOUCH) {
        touchPosition tp;
        hidTouchRead(&tp);
        s->dragging = true;
        s->last_touch_y = (int)tp.py;
    } else if ((kHeld & KEY_TOUCH) && s->dragging) {
        touchPosition tp;
        hidTouchRead(&tp);
        int dy = s->last_touch_y - (int)tp.py;
        if (dy >= 18) {
            s->offset++;
            s->last_touch_y = (int)tp.py;
        } else if (dy <= -18) {
            s->offset--;
            s->last_touch_y = (int)tp.py;
        }
    }
    if (kUp & KEY_TOUCH) {
        s->dragging = false;
    }

    int max_offset = item_count - visible_rows;
    if (max_offset < 0) max_offset = 0;
    if (s->offset < 0) s->offset = 0;
    if (s->offset > max_offset) s->offset = max_offset;
}

#endif // !HOST_BUILD
