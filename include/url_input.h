#ifndef URL_INPUT_H
#define URL_INPUT_H

#include <stdbool.h>
#include <stddef.h>

// Validate a user-supplied URL.
//
// Returns true iff:
//   - `url` is non-NULL,
//   - starts with `http://` or `https://`,
//   - has at least one character after the scheme,
//   - is shorter than 512 bytes (`MAX_URL_LENGTH`).
//
// Pure helper, host-testable.
bool url_is_valid_http(const char* url);

#ifndef HOST_BUILD
// Open the system software keyboard (swkbd) to prompt the user for a URL.
// On success, writes the entered URL (NUL-terminated) into `out` (capped
// to `out_sz` including the NUL) and returns true. Returns false if the
// user cancelled or swkbd reported an error.
bool url_input_prompt(char* out, size_t out_sz);
#endif

// Append `url` as a new line to the configuration file at `path`,
// preceded by a `# added in-app YYYY-MM-DD` comment. Returns true on
// success. The function is purely additive: existing lines are never
// rewritten, reordered, or removed (config v1.0 compatibility).
bool url_input_append_to_config(const char* path, const char* url);

#endif // URL_INPUT_H
