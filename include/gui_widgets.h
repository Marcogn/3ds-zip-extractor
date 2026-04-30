#ifndef GUI_WIDGETS_H
#define GUI_WIDGETS_H

#include <stdbool.h>

#ifndef HOST_BUILD
#include <3ds.h>
#endif

// A clickable rectangular button. `label` is rendered centered.
typedef struct {
    float x, y, w, h;
    const char* label;
} gui_button_t;

// Cooperative scroll state used by the bottom-screen interactive lists.
// Tracked across frames; updated by gui_scroll_update().
typedef struct {
    int  offset;          // index of the topmost visible item
    int  last_touch_y;    // last touch y while dragging (in pixels)
    bool dragging;        // true while a touch is held down
} gui_scroll_t;

#ifndef HOST_BUILD
// Returns true iff `tp` lies inside `btn`. Both inputs may be NULL → false.
bool gui_button_hit(const gui_button_t* btn, const touchPosition* tp);

// Render a button. `pressed` darkens the fill to indicate touch feedback.
void gui_widgets_draw_button(const gui_button_t* btn, bool pressed);

// Render a horizontal action bar (uniform height, equal-width buttons)
// at the bottom of the bottom screen.
void gui_widgets_draw_action_bar(const gui_button_t* buttons, int n);

// Update `s->offset` based on D-Pad / circle-pad input plus optional
// touch dragging. The function is cooperative: call it once per frame.
//
// `item_count` is the total number of list items, `visible_rows` is how
// many rows fit on screen. The function clamps `offset` to a valid range.
void gui_scroll_update(gui_scroll_t* s, int item_count, int visible_rows);
#endif

#endif // GUI_WIDGETS_H
