# Architecture

> Snapshot of the post-refactor layout. Use this as the high-level map
> when adding a feature or tracking down a bug.

## 1. Module map

```
                    ┌────────────────────────────────┐
                    │  source/main.c                 │
                    │  state machine, app lifecycle  │
                    └───────────────┬────────────────┘
                                    │
        ┌───────────┬───────────────┼───────────────┬─────────────┐
        ▼           ▼               ▼               ▼             ▼
  ┌─────────┐  ┌────────┐  ┌──────────────┐  ┌────────────┐  ┌─────────┐
  │ config  │  │ gdrive │  │ download     │  │ archive_*  │  │ queue   │
  │ (pure)  │  │ (pure) │  │ (libcurl)    │  │ extractor  │  │ (UI)    │
  └────┬────┘  └────────┘  └──────┬───────┘  └────┬───────┘  └─────────┘
       │                          │               │
       │                          ▼               ▼
       │                   ┌────────────┐  ┌────────────────┐
       │                   │  libcurl   │  │   libarchive   │
       │                   │  + mbedtls │  │  + zlib/bz2/   │
       │                   └────────────┘  │   xz/zstd      │
       │                                   └────────────────┘
       ▼
  ┌──────────┐  ┌────────┐  ┌──────────────┐  ┌──────┐
  │integrity │  │  led   │  │file_browser  │  │ gui  │
  │(mbedtls/ │  │ (MCU   │  │ (sdmc files) │  │(C2D) │
  │  pure)   │  │  hook) │  └──────────────┘  └──────┘
  └──────────┘  └────────┘
```

## 2. Module responsibilities

| Module | File(s) | Pure? | Responsibility |
| --- | --- | --- | --- |
| `config` | `source/config.c`, `include/config.h` | ✅ | Parse `config.txt`, apply defaults, expose `AppConfig` (settings + URL list). Backwards compatible with v1.0. |
| `gdrive` | `source/gdrive.c`, `include/gdrive.h` | ✅ | Convert Google Drive sharing URLs into direct-download URLs. Pass-through for everything else. |
| `integrity` | `source/integrity.c`, `include/integrity.h` | ✅ | SHA-256 of buffers/files, optional integrity check. Uses mbedtls on target, a vendored pure-C implementation on host. |
| `archive_extractor_detect` | `source/archive_extractor_detect.c`, `include/archive_extractor.h` | ✅ | Magic-byte sniffing for ZIP/TAR/GZIP/BZIP2/XZ/ZSTD/7Z/RAR. No libarchive dependency, host-testable. |
| `archive_extractor` | `source/archive_extractor.c` | ❌ (libarchive) | Driver around `libarchive` for extraction. Ships honest stubs for `umask`/`getpwnam`/`getgrnam`/LZ4 missing on devkitARM. |
| `download` | `source/download.c`, `include/download.h` | ❌ (libcurl) | Run a `curl` transfer to a unique `*.part` file under `sdmc:/3ds/zip-extractor/tmp/`. Implements resume (Range/206) with 200 fallback, configurable buffer + connect timeout. |
| `led` | `source/led.c`, `include/led.h` | ❌ (libctru) | Thin wrapper for the MCUHWC LED notifications. |
| `file_browser` | `source/file_browser.c`, `include/file_browser.h` | ❌ (citro2d) | SD card directory browser (used to choose extraction path). |
| `queue` | `source/queue.c`, `include/queue.h` | ❌ (citro2d) | Renders the download/queue status screen on the top screen. `DownloadQueue` is just an alias for `AppConfig`. |
| `gui` | `source/gui.c`, `include/gui.h` | ❌ (citro2d/3d) | Common rendering helpers (status, menu, progress, error). Owns the dark-blue theme constants. Hosts the persistent **bottom-screen context** (queue + virtual action bar) reused by every top state. |
| `gui_widgets` | `source/gui_widgets.c`, `include/gui_widgets.h` | ❌ (citro2d) | Reusable touch widgets: `gui_button_t` hit-test, button rendering, the 5-button action bar, and the cooperative `gui_scroll_t` D-Pad/touch-drag scroll helper. |
| `url_input` | `source/url_input.c`, `include/url_input.h` | ✅ (validator) / ❌ (`url_input_prompt` swkbd) | `url_is_valid_http` is host-tested; the swkbd prompt and the additive `url_input_append_to_config` writer are target-only. |
| `speed_meter` | `source/speed_meter.c`, `include/speed_meter.h` | ✅ | Rolling 2-second / 16-sample throughput estimator powering the download `MB/s` + ETA display. Used by `download.c`. |
| `eta_format` | `source/eta_format.c`, `include/eta_format.h` | ✅ | `format_eta` — `mm:ss` formatter, capped at `99:59`, `--:--` for negatives. Used by `gui.c`. |
| `main` | `source/main.c` | ❌ | App lifecycle: init/shutdown, state machine `MENU → BROWSER/QUEUE/RUNNING → SUMMARY → MENU/EXIT`, single-thread cooperative loop. |

## 3. Build pipeline

- Target: `make` invokes the devkitARM rules from `$DEVKITARM/3ds_rules`.
  All `source/*.c` are compiled and linked against
  `libcitro2d`, `libcitro3d`, `libcurl`, `libmbedtls`, `libarchive`,
  `liblzma`, `libbz2`, `libzstd`, `libz`, `libctru`. Output:
  `3ds-zip-extractor.{elf,3dsx,smdh}`.
- Host tests: `make -C tests -f Makefile.host` compiles only the *pure*
  modules (`config`, `gdrive`, `integrity`, `archive_extractor_detect`)
  plus vendored Unity, with `-DHOST_BUILD=1`. No 3DS dependency.

## 4. Data flow for a single URL

```
config.txt ──► read_config_file ──► AppConfig.items[i]
                                          │ (url, sha256_hex, state)
                                          ▼
                              download_make_temp_path
                                          │
                                          ▼
                       convert_gdrive_url ─► download_file ──► curl
                                          │ (resume / 200 fallback)
                                          ▼
                             verify_integrity (if hash present)
                                          │
                                          ▼
                                detect_archive_type
                                          │
                                          ▼
                            extract_archive_libarchive
                                          │
                                          ▼
                              led_notification_green
```

## 5. Threading model

The application is intentionally **single-thread cooperative**.
`aptMainLoop()` drives the main loop; long operations (`curl_easy_perform`,
`extract_archive_libarchive`) yield only inside their own libraries.
The progress/cancel callbacks call `hidScanInput` once per chunk to
keep the UI responsive. Adding threads would require careful work
around APT/HID/citro2d ownership and is explicitly out of scope.
