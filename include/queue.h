#ifndef QUEUE_H
#define QUEUE_H

#include "config.h"

// `DownloadQueue` is an alias for `AppConfig`. The two concepts are tightly
// coupled (settings + the URL list with per-item state) so we keep a single
// underlying type. The alias preserves the original naming used throughout
// the codebase.
typedef AppConfig DownloadQueue;

// Render the queue/status screen for `current_page` (paged 12 items per
// page). Implementation lives in queue.c (uses citro2d). On host builds
// this is unavailable.
#ifndef HOST_BUILD
void display_queue_status(DownloadQueue* queue, int current_page);
#endif

#endif // QUEUE_H
