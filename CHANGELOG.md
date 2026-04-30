# Changelog

## [Unreleased]

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
