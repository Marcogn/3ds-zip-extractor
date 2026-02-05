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

# Add URLs below
https://example.com/file.zip
```

---

## Controls

| Button | Action |
|--------|--------|
| A | Start/Confirm |
| B | Cancel |
| X | View queue |
| SELECT | File browser |
| START | Exit |
| D-Pad | Navigate |

---

## Building

```bash
# Requires devkitPRO
make
```

---

## License

MIT License - see [LICENSE](LICENSE)

---

## Links

- [Changelog](CHANGELOG.md)
- [Contributing](CONTRIBUTING.md)
