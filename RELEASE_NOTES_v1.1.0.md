# 3DS Archive Extractor v1.1.0

**Phase 3 UX release** — April 30, 2026

This release rebuilds the user experience around the touchscreen.
The bottom screen is now a persistent interactive queue with a virtual
action bar, downloads show a live speed meter and ETA, and you can both
**add URLs in-app via swkbd** and **extract local archives directly
from the file browser**.

---

## ✨ What's New

### Interactive bottom queue + virtual action bar
The bottom screen now lists every URL in the queue with colored status
glyphs (`o` pending, `>` running, `v` done, `x` failed, `-` skipped).

- **Touch a row** to toggle `pending ⇄ skipped` — the running item is
  protected and can't be toggled by accident.
- **Drag** vertically (or use the D-Pad / Circle Pad) to scroll long
  queues.
- **Touch the action-bar buttons** at the foot of the screen to fire
  the same actions as the physical `A B X Y START` keys.
- Header shows `Queue (count/MAX_URLS)` and a `^v` scroll hint.

### In-app URL entry via the system keyboard
Press **Y** from the main menu to open swkbd and add a new URL
on-the-fly. After validation (`url_is_valid_http` rejects non-`http(s)`
schemes, empty input, and overly-long strings), a 3-second banner
offers `R + A` to **also persist the URL into `config.txt`** — purely
additive: existing config lines are never rewritten or reordered, so
v1.0 configs stay readable.

### Local archive extraction from the file browser
Pressing **A** on a file in the SD card browser now runs the same
secure libarchive pipeline used for downloads:

1. Detect format via magic bytes.
2. Pre-count entries for an accurate progress bar.
3. Extract into the configured `extract_path` with the same
   `ARCHIVE_EXTRACT_SECURE_*` flags as the download path.

Unsupported files show a 1.5-second tooltip; the source archive is
**not** deleted (unlike the temp file from a download).

### Rolling speed meter and live ETA
Downloads now show:

```
12.34 MB / 50.00 MB  (24.7%)
[█████░░░░░░░░░░░░░░░]
1.23 MB/s   ETA 02:34
```

The speed is computed from a 16-sample / 2-second sliding window so
brief network hiccups don't make the figure jump. The ETA is capped at
`99:59` and falls back to `--:--` when bandwidth isn't yet known.

### Percentage progress bar during extraction
Whenever libarchive can pre-count entries, the extractor draws a real
percentage bar:

```
Files: 123 / 456
[████████░░░░░░░░░░░]
27.0%
```

For non-seekable streams (e.g. `.tar.gz` over an unknown size), the bar
falls back to an animated spinner over the running file count.

---

## 🧪 Testing

The pure helpers ship with three new host test suites:

| Module        | Tests | Coverage |
| ------------- | ----- | -------- |
| `url_input`   | 13    | URL validation edge cases + additive config writer |
| `speed_meter` | 7     | Empty / single-sample / window pruning / ring overflow / null safety |
| `eta_format`  | 7     | Negative, cap, short-buffer, NULL guards |

Total off-target coverage is now **50 cases** under ASan/UBSan, run by
the existing CI.

---

## 🛠 Compatibility

- **Config format**: still backwards compatible with v1.0. New URLs
  added via the in-app prompt are appended after a `# added in-app
  YYYY-MM-DD` comment.
- **Controls**: physical-button mappings are unchanged; the virtual
  action bar **replicates** them, it doesn't replace them.
- **Network stack**: `User-Agent` is now `3DS-Zip-Extractor/1.1.0`;
  TLS handling, resume, and Google Drive URL conversion are unchanged.

---

## 📋 Known Manual-Only Items

These are intrinsically hard to test off-target and were verified on
Citra/Lime3DS:

- The swkbd UI itself (only `url_is_valid_http` is host-tested).
- Touch-drag scrolling on the bottom queue and action-bar hit-tests.
- The MCU LED notification fired at the end of a local extraction.
- The exact rendering of the `COLOR_*` palette on real 3DS hardware.

---

## 📦 Installation

Same as v1.0:

1. Copy `3ds-zip-extractor.3dsx` to `/3ds/` on the SD card.
2. Launch from the Homebrew Launcher.

The first run still creates a default `config.txt`; existing v1.0
configs are picked up unchanged.

---

## 🔗 Links

- [Repository](https://github.com/Marcogn/3ds-zip-extractor)
- [Changelog](CHANGELOG.md)
- [Architecture](docs/ARCHITECTURE.md)
- [Issues](https://github.com/Marcogn/3ds-zip-extractor/issues)

---

**Enjoy the new touch-first workflow!** Bug reports and feedback are
welcome on the [Issues page](https://github.com/Marcogn/3ds-zip-extractor/issues).
