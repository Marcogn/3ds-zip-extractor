#include "url_input.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "config.h"  // MAX_URL_LENGTH

#ifndef HOST_BUILD
#include <3ds.h>
#endif

bool url_is_valid_http(const char* url) {
    if (!url) return false;
    size_t len = strlen(url);
    if (len == 0) return false;
    if (len >= MAX_URL_LENGTH) return false;  // 512-byte buffer cap

    static const char http[]  = "http://";
    static const char https[] = "https://";

    if (len > sizeof(http) - 1 && strncmp(url, http, sizeof(http) - 1) == 0) {
        return url[sizeof(http) - 1] != '\0';
    }
    if (len > sizeof(https) - 1 && strncmp(url, https, sizeof(https) - 1) == 0) {
        return url[sizeof(https) - 1] != '\0';
    }
    return false;
}

#ifndef HOST_BUILD
bool url_input_prompt(char* out, size_t out_sz) {
    if (!out || out_sz < 8) return false;

    // 512-byte buffer matches MAX_URL_LENGTH; clamp to caller's buffer.
    size_t buf_sz = out_sz < MAX_URL_LENGTH ? out_sz : MAX_URL_LENGTH;
    char buf[MAX_URL_LENGTH];
    memset(buf, 0, sizeof(buf));

    SwkbdState swkbd;
    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, (int)buf_sz - 1);
    swkbdSetHintText(&swkbd, "Enter URL to download");
    swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT,  "Cancel", false);
    swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "OK",     true);
    swkbdSetValidation(&swkbd, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);

    SwkbdButton btn = swkbdInputText(&swkbd, buf, buf_sz);
    if (btn != SWKBD_BUTTON_RIGHT) return false;

    buf[buf_sz - 1] = '\0';
    strncpy(out, buf, out_sz - 1);
    out[out_sz - 1] = '\0';
    return true;
}
#endif

bool url_input_append_to_config(const char* path, const char* url) {
    if (!path || !url) return false;
    if (!url_is_valid_http(url)) return false;

    FILE* f = fopen(path, "ab");
    if (!f) return false;

    // ISO-like date stamp; on host or 3DS the C library provides time().
    time_t t = time(NULL);
    struct tm* tm_now = (t == (time_t)-1) ? NULL : localtime(&t);
    char date[16] = "0000-00-00";
    if (tm_now) {
        strftime(date, sizeof(date), "%Y-%m-%d", tm_now);
    }

    // Preceding newline: makes append safe whether or not the file ended
    // with one. Two newlines max — never break previous content.
    fprintf(f, "\n# added in-app %s\n%s\n", date, url);
    fflush(f);
    fclose(f);
    return true;
}
