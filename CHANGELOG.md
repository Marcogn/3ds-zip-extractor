# Changelog

## [Unreleased]

### Added
- **`tests/test_integrity.c`** — 8-case host test suite for `verify_integrity()`:
  correct hash, wrong hash, missing path, short hash, long hash, non-hex chars,
  empty file, and uppercase hex accepted case-insensitively. Temp files are
  created with `mkstemp()`; no `sdmc:` paths used.
- **Config clamping tests** — 5 new cases in `tests/test_config.c` covering
  `max_urls=0` (→ 1), `max_urls=99999` (→ 100), `download_buffer_kb=abc` (→ 4),
  `connect_timeout_s=-5` (→ 1), and unknown key + known key (parsed correctly).
- **`CODE_OF_CONDUCT.md`** — Contributor Covenant v2.1 at the repository root.

### Changed
- Release notes moved from root to `docs/release-notes/`
  (`RELEASE_NOTES_v1.0.md`, `RELEASE_NOTES_v1.1.0.md`).

## [1.1.0] - 2026-04-30

### Added — Phase 3 UX/GUI
- **Interactive bottom queue.** The bottom screen is now a persistent
  scrollable list of every URL with colored status icons (pending,
  in-progress, completed, failed, skipped). Touch a row to toggle
  pending ↔ skipped. A virtual action bar at the foot of the screen
  mirrors the physical buttons (`A B X Y START`).
- **In-app URL entry.** Pressing `Y` on the menu opens the system
  software keyboard (`swkbd`) to add a URL on-the-fly. After validation
  (`url_is_valid_http`), a 3-second banner offers `R + A` to also
  persist the URL into `config.txt` (purely additive, never rewrites
  existing lines).
- **Local archive extraction from the file browser.** Pressing `A` on a
  supported archive prompts for confirmation and extracts directly into
  `extract_path` — no download required. Unsupported files show a
  1.5-second tooltip.
- **Rolling speed meter and ETA.** Downloads now show a 2-second
  windowed `MB/s` figure and a live `mm:ss` ETA (capped at `99:59`,
  `--:--` when unknown). Implemented as a 16-sample ring buffer in the
  pure `speed_meter` module.
- **Percentage extraction progress.** The extractor pre-counts archive
  entries (`archive_count_entries`) so the GUI can render a real
  percentage bar; falls back to an animated spinner when the count
  isn't available (e.g. non-seekable streams).
- **Cooperative timed overlays.** `gui_draw_error_timed`,
  `gui_tooltip`, and `gui_confirm_persist_url` keep `aptMainLoop()`
  ticking while a transient message is on-screen.
- **New host test suites.** `test_url_input` (13 cases),
  `test_speed_meter` (7 cases), `test_eta_format` (7 cases). Total
  off-target coverage is now 50 cases under ASan/UBSan.

### Changed
- **Bumped HTTP `User-Agent`** to `3DS-Zip-Extractor/1.1.0`.
- **Version badge** in `README.md` updated to `1.1.0`.
- **File browser legend** updated to reflect the new local-extract
  action (`A: Enter/Extract`, `Y: Use as path`, `X: Refresh`).

### Notes
- The bottom-screen rendering is shared by every top-screen state
  through a small "bottom context" registered once at startup
  (`gui_set_bottom_context`). The active item is tracked via
  `gui_set_bottom_active` while a download is running.
- swkbd UI, real touch dragging, LED MCU notifications and the exact
  on-device colour rendering remain manually verified — only the pure
  helpers (`url_is_valid_http`, `speed_meter_*`, `format_eta`) are
  exercised by the host test suite.

---

## [1.0.1-refactor]

### Added
- **Modular source layout.** `source/main.c` has been split into focused
  modules with public headers in `include/`:
  `led`, `gdrive`, `config`, `download`, `file_browser`, `queue`,
  `integrity`, plus a pure `archive_extractor_detect` module that's
  reusable on the host.
- **Optional integrity check.** A URL line in `config.txt` may now end with
  ` sha256:<64-hex>` to verify the downloaded file. Mismatches mark the
  item as failed without extracting.
- **Configurable runtime limits.** New optional config keys
  `max_urls`, `download_buffer_kb`, `connect_timeout_s` (defaults: 100,
  128, 30). Values out of range are clamped, never rejected.
- **Per-run unique temp paths.** Downloads now land in
  `sdmc:/3ds/zip-extractor/tmp/run_<ts>_<n>.part` instead of a single
  shared `temp_download.tmp`. Orphan `.part` files are cleaned up at
  startup and shutdown.
- **Honest stubs.** `umask` / `getpwnam` / `getgrnam` / `LZ4_*` now log a
  one-shot warning to `stderr` the first time they're hit so unexpected
  use is visible.
- **Off-target unit tests.** A new `tests/` directory builds with a plain
  host C compiler (no devkitARM, no 3DS) and exercises the pure modules
  with vendored Unity. Run with `make -C tests -f Makefile.host test`
  (or `test-asan` for ASan/UBSan, or `coverage` for an `lcov` HTML
  report).
- **CI**. New `.github/workflows/build.yml` builds the `.3dsx` in the
  `devkitpro/devkitarm:latest` image, runs the host test suite under
  ASan/UBSan, uploads `.3dsx`/`.smdh` as build artifacts, and publishes
  a GitHub Release on `v*` tags.

### Changed
- **Hardened extraction.** libarchive is now configured with
  `ARCHIVE_EXTRACT_SECURE_NODOTDOT |
  ARCHIVE_EXTRACT_SECURE_SYMLINKS |
  ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS` to mitigate zip-slip and
  symlink-based attacks on top of the previous protections.
- **Resume hardening.** The HTTP resume path now detects servers that
  reply with `200 OK` instead of `206 Partial Content` and restarts the
  download from zero rather than appending to a stale prefix.
- **Config parser.** Lines longer than `MAX_URL_LENGTH` are now drained
  to the next `\n` and recorded as a single (truncated, NUL-terminated)
  entry instead of splitting into bogus extra "URLs". UTF-8 BOMs on the
  first line are skipped.
- **Build artifacts no longer tracked.** `.3dsx` and `.smdh` are now
  ignored by git and produced fresh by every build; releases ship them
  as Release assets.

### Removed
- The hand-rolled ZIP/zlib path (`extract_zip_file`, `mkdir_recursive`,
  `ZIP_LOCAL_SIGNATURE`, manual `inflate`) is gone from `main.c`. All
  archive types — ZIP included — now go through the libarchive
  extractor.

---

## [1.0] - 2026-02-05

### First Stable Release

#### Features
- Graphical interface with citro2d/citro3d (dark blue theme)
- Multi-format archive support via libarchive
- HTTP/HTTPS downloads via libcurl
- Google Drive URL conversion
- Auto-config creation on first run
- Sleep mode support
- LED notifications (green=complete, pink=download done)
- File browser for extraction path
- Download queue system
- Auto-retry for failed downloads

#### Supported Formats
- ZIP, TAR, 7Z, RAR (read-only)
- TAR.GZ, TAR.BZ2, TAR.XZ, TAR.ZSTD
- GZIP, BZIP2, XZ, ZSTD

#### Known Limitations
- Large archives (>200MB) may cause issues on Old 3DS
- Password-protected archives not supported
- LZ4 format not supported

---

Report bugs at: https://github.com/Marcogn/3ds-zip-extractor/issues
