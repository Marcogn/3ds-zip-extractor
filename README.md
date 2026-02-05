# 3DS Archive Extractor

<p align="center">
  <img src="icon.png" alt="3DS Archive Extractor Icon" width="128">
</p>

Download and extract compressed archives directly on your Nintendo 3DS. Supports 11+ archive formats including ZIP, TAR, 7Z, and RAR.

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Nintendo%203DS-red.svg)](https://www.nintendo.com/3ds/)
[![Version](https://img.shields.io/badge/Version-2.0.2-green.svg)](CHANGELOG.md)

---

## Features

- **Multi-format support**: Extract ZIP, TAR, 7Z, RAR, GZIP, BZIP2, XZ, ZSTD and more
- **Direct downloads**: Download files from HTTP/HTTPS URLs directly on your 3DS
- **Google Drive support**: Automatic URL conversion for Google Drive links
- **Auto-retry**: Automatically retry failed downloads (configurable)
- **File browser**: Choose extraction directory with built-in browser
- **Queue management**: Process multiple files in sequence
- **Console interface**: Stable text-based interface optimized for 3DS hardware
- **Auto-config**: Creates example config file on first run

---

## Supported Formats

| Format | Extensions | Notes |
|--------|-----------|-------|
| **ZIP** | .zip | Full support (DEFLATE/STORE) |
| **TAR** | .tar | Uncompressed archives |
| **TAR.GZ** | .tar.gz, .tgz | GZIP compressed |
| **TAR.BZ2** | .tar.bz2, .tbz2 | BZIP2 compressed |
| **TAR.XZ** | .tar.xz, .txz | LZMA2 compressed |
| **TAR.ZSTD** | .tar.zst | Zstandard compressed |
| **7-Zip** | .7z | Full support |
| **RAR** | .rar | Read-only (extraction only) |
| **GZIP** | .gz | Single file compression |
| **BZIP2** | .bz2 | Single file compression |
| **XZ** | .xz | Single file compression |
| **ZSTD** | .zst | Single file compression |

**Note**: LZ4 format is not supported (library unavailable on 3DS platform).

---

## Installation

### Requirements

- Nintendo 3DS / 2DS / New 3DS with **Homebrew Launcher**
- **WiFi** connection for downloads
- **SD card** with free space

### Quick Start

1. Download the latest `3ds-zip-extractor.3dsx` from the releases
2. Copy it to `/3ds/` folder on your SD card
3. Launch from Homebrew Launcher
4. On first run, the app creates a config file at `/3ds/zip-extractor/config.txt`
5. Edit the config file to add your download URLs
6. Restart the app to begin downloading

---

## Usage

### Configuration

Edit `/3ds/zip-extractor/config.txt` on your SD card:

```ini
# Extract directory (must end with /)
extract_path=sdmc:/extracted/

# Automatically retry failed downloads
auto_retry=true
max_retries=3

# URLs of files to download (one per line)
https://example.com/file.zip
https://example.com/archive.tar.gz
https://drive.google.com/file/d/FILE_ID/view
```

### Controls

| Button | Action |
|--------|--------|
| **A** | Start downloads/extraction |
| **B** | Cancel current operation |
| **X** | View download queue |
| **Y** | Skip failed downloads |
| **SELECT** | Open file browser |
| **START** | Exit application |
| **L/R** | Navigate pages (in queue view) |
| **D-Pad** | Navigate files (in browser) |

---

## Hardware Compatibility

| Console | Performance | Notes |
|---------|-------------|-------|
| Old 3DS / 3DS XL / 2DS | Basic | Works well for files <50MB |
| New 3DS / New 3DS XL / New 2DS XL | Fast | Recommended for large archives |

**Performance notes**:
- Old 3DS: 64MB RAM, 268MHz CPU - suitable for most use cases
- New 3DS: 128MB RAM, 804MHz CPU - handles large archives easily
- For archives >50MB, New 3DS is strongly recommended

---

## Building from Source

### Prerequisites
```bash
# Install devkitPRO and libraries
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-libarchive \
                   3ds-zlib 3ds-bzip2 3ds-xz 3ds-zstd
```

### Build
```bash
git clone https://github.com/Marcogn/3ds-zip-extractor.git
cd 3ds-zip-extractor
make
```

Output: `3ds-zip-extractor.3dsx`

---

## Documentation

- [CHANGELOG.md](CHANGELOG.md) - Version history and release notes
- [CONTRIBUTING.md](CONTRIBUTING.md) - How to contribute to the project
- [SECURITY.md](SECURITY.md) - Security policy and vulnerability reporting
- [LICENSE](LICENSE) - MIT License terms

---

## Troubleshooting

**Download fails**:
- Check your WiFi connection
- Verify the URL works in a browser
- Enable `auto_retry=true` in config.txt
- For Google Drive, ensure the file is publicly accessible

**Extraction fails**:
- Check available SD card space
- Verify the archive format is supported
- Try re-downloading (file might be corrupted)
- Password-protected archives are not supported

**App crashes**:
- Archive might be too large for Old 3DS (try New 3DS)
- Close other homebrew apps to free RAM
- Ensure you have enough SD card space

For more help, open an [Issue](https://github.com/Marcogn/3ds-zip-extractor/issues).

---

## Google Drive Support

The app automatically converts Google Drive sharing URLs to direct download links.

Supported formats:
```
https://drive.google.com/file/d/FILE_ID/view
https://drive.google.com/open?id=FILE_ID
```

Example: Simply paste `https://drive.google.com/file/d/1ABC123xyz/view` in config.txt

---

## Technical Details

Built with:
- **libarchive** - Multi-format archive extraction
- **libcurl** - HTTP/HTTPS downloads
- **mbedtls** - SSL/TLS support
- **zlib, libbz2, liblzma, libzstd** - Compression libraries
- **libctru** - Nintendo 3DS system library

---

## FAQ

**Can I extract password-protected archives?**  
No, password-protected archives are not supported.

**What's the maximum file size?**  
Limited by RAM and SD card space. Archives >200MB may cause issues on Old 3DS.

**Does it work with Mega.nz or Dropbox?**  
Yes, if you provide a direct download URL (not a sharing page).

**Can I create archives?**  
No, only extraction is supported.

---

## Contributing

Contributions are welcome. Fork the repository, create a branch, commit your changes, and open a pull request.

See [CONTRIBUTING.md](CONTRIBUTING.md) for details. For version history, see [CHANGELOG.md](CHANGELOG.md).

---

## License

MIT License - see [LICENSE](LICENSE) for details.

---

## Credits

- devkitPRO team - 3DS toolchain and libraries
- libarchive team - Multi-format archive library
- 3DS homebrew community

**Repository**: [github.com/Marcogn/3ds-zip-extractor](https://github.com/Marcogn/3ds-zip-extractor)

