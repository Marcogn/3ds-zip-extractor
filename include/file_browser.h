#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include <stdbool.h>

#include "config.h"  // MAX_PATH_LENGTH

#define MAX_DIR_ENTRIES 100

typedef struct {
    char name[256];
    bool is_directory;
} DirEntry;

typedef struct {
    DirEntry entries[MAX_DIR_ENTRIES];
    int  count;
    int  selected;
    char current_path[MAX_PATH_LENGTH];
} FileBrowser;

// Initialise the browser at the given starting path.
void init_file_browser(FileBrowser* browser, const char* start_path);

// Load the contents of `browser->current_path`. Returns the entry count,
// or -1 if the directory cannot be opened. Entries are sorted with
// directories first, then alphabetically.
int load_directory(FileBrowser* browser);

// Render the browser screen. citro2d-only; not built on host.
#ifndef HOST_BUILD
void display_file_browser(FileBrowser* browser, int scroll_offset);
#endif

#endif // FILE_BROWSER_H
