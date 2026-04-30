# 3DS Archive Extractor

<p align="center">
  <img src="icon.png" alt="3DS Archive Extractor Icon" width="128">
</p>

Download and extract archives directly on your Nintendo 3DS.

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Nintendo%203DS-red.svg)](https://www.nintendo.com/3ds/)
[![Version](https://img.shields.io/badge/Version-1.0-green.svg)](CHANGELOG.md)

> **First stable release** - Download and extract archives on Nintendo 3DS

---

## Features

- Download files from HTTP/HTTPS URLs
- Extract ZIP, TAR, 7Z, RAR and other formats
- Google Drive URL support
- Dark themed GUI (citro2d)
- Sleep mode downloads
- LED notifications

---

## Supported Formats

ZIP, TAR, TAR.GZ, TAR.BZ2, TAR.XZ, TAR.ZSTD, 7Z, RAR (read-only), GZIP, BZIP2, XZ, ZSTD

---

## Installation

1. Copy `3ds-zip-extractor.3dsx` to `/3ds/` on your SD card
2. Launch from Homebrew Launcher
3. On first run, config file is created at `/3ds/zip-extractor/config.txt`
4. Edit config file to add URLs
5. Restart the app

---

## Configuration

Edit `/3ds/zip-extractor/config.txt`:

```ini
extract_path=sdmc:/extracted/
auto_retry=true
max_retries=3

# Optional limits (defaults shown)
# max_urls=100
# download_buffer_kb=128
# connect_timeout_s=30

# Add URLs below
https://example.com/file.zip

# Optional: integrity check — append " sha256:<64-hex>" to a URL
https://example.com/strict.zip sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
```

The format is **backwards-compatible with v1.0** config files.

---

## Controls

| Button | Action |
|--------|--------|
| A | Start / Confirm |
| B | Cancel |
| X | View queue |
| Y | (in queue view) Mark failed items as skipped |
| L / R | (in queue view) Page through URLs |
| SELECT | File browser |
| START | Exit |
| D-Pad | Navigate |

---

## Building

### Native (devkitPRO)

```bash
# Requires devkitPRO + devkitARM + portlibs 3DS
make
```

### Docker (no local devkitPRO needed)

```bash
docker run --rm -v "$PWD":/src -w /src devkitpro/devkitarm:latest make
```

This is the same image used by CI; useful for reproducible builds.

### Host unit tests

The pure modules (`config`, `gdrive`, `integrity`, archive detection)
have an off-target unit-test suite that runs on a normal Linux/macOS
PC — no devkitARM, no 3DS:

```bash
make -C tests -f Makefile.host test         # plain build
make -C tests -f Makefile.host test-asan    # with AddressSanitizer + UBSan
make -C tests -f Makefile.host coverage     # gcov/lcov HTML report
```

The CI pipeline runs both `build-3ds` (Docker) and `host-tests`
(ASan/UBSan); both must be green for a PR to be merged.

---

## License

MIT License - see [LICENSE](LICENSE)

---

## Links

- [Changelog](CHANGELOG.md)
- [Architecture](docs/ARCHITECTURE.md)
- [Contributing](CONTRIBUTING.md)
