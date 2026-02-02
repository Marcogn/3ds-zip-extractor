# 3DS Archive Extractor

<p align="center">
  <img src="icon.png" alt="3DS Archive Extractor" width="128">
</p>

<p align="center">
  <strong>Download and extract compressed archives directly on your Nintendo 3DS</strong><br>
  <em>Supports 11+ archive formats</em>
</p>

<p align="center">
  <a href="#features">Features</a> •
  <a href="#supported-formats">Formats</a> •
  <a href="#installation">Installation</a> •
  <a href="#usage">Usage</a> •
  <a href="#documentation">Documentation</a>
</p>

---

## Features
✨ **Multi-format Support** - Extract ZIP, TAR, 7Z, RAR, GZIP, BZIP2, XZ and more  
📥 **Direct Download** - Download files from HTTP/HTTPS directly on your 3DS  
🔄 **Auto-retry** - Automatically retry failed downloads  
📂 **File Browser** - Choose where to extract files  
🎮 **Native GUI** - Optimized graphical interface for 3DS  
⚡ **Efficient** - Works on Old 3DS, fast on New 3DS  
☁️ **Google Drive** - Automatic URL conversion for Google Drive links

---

## Supported Formats
| Format | Extension | Compression |
|--------|-----------|-------------|
| **ZIP** | `.zip` | DEFLATE/STORE |
| **TAR** | `.tar` | None |
| **TAR.GZ** | `.tar.gz`, `.tgz` | GZIP |
| **TAR.BZ2** | `.tar.bz2`, `.tbz2` | BZIP2 |
| **TAR.XZ** | `.tar.xz`, `.txz` | LZMA2 |
| **TAR.ZSTD** | `.tar.zst` | Zstandard |
| **7-Zip** | `.7z` | LZMA |
| **RAR** | `.rar` | RAR (read-only) |
| **GZIP** | `.gz` | DEFLATE |
| **BZIP2** | `.bz2` | Burrows-Wheeler |
| **XZ** | `.xz` | LZMA2 |
| **ZSTD** | `.zst` | Zstandard |

> **Note**: LZ4 format is not supported (library unavailable on 3DS)

---

## Installation

### Requirements

- Nintendo 3DS / 2DS / New 3DS with **Homebrew Launcher**
- **WiFi** connection
- **SD card** with free space

### Steps

1. **Download** the latest `3ds-zip-extractor.3dsx` from [Releases](https://github.com/Marcogn/3ds-zip-extractor/releases)

2. **Copy** the file to the `/3ds/` folder on your SD card

3. **Create** the directory and configuration file:
   ```
   SD:/3ds/zip-extractor/config.txt
   ```

4. **Configure** the `config.txt` file (see example below)

5. **Launch** the app from Homebrew Launcher

---

## Usage

### Basic Configuration

Create `/3ds/zip-extractor/config.txt` on your SD card:
```ini
# Extract directory (must end with /)
extract_path=sdmc:/extracted/
# Automatically retry failed downloads
auto_retry=true
max_retries=3
# URLs of files to download (one per line)
https://example.com/file.zip
https://example.com/archive.tar.gz
https://example.com/data.7z
```

### Advanced Configuration

```ini
extract_path=sdmc:/downloads/
auto_retry=true
max_retries=5
# HTTP/HTTPS supported
https://example.com/release.zip
# Google Drive (automatic URL conversion)
https://drive.google.com/file/d/FILE_ID/view
# Multiple formats
https://example.com/backup.tar.bz2
https://example.com/archive.7z
https://example.com/data.tar.xz
```

### Controls

| Button | Action |
|--------|--------|
| **A** | Start downloads/extraction |
| **B** | Cancel current operation |
| **X** | View download queue |
| **SELECT** | Open file browser |
| **START** | Exit application |
| **L/R** | Navigate pages (queue view) |

---

## Compatibility

### Hardware
| Console | RAM | CPU | Performance |
|---------|-----|-----|-------------|
| 3DS Old | 64MB | 268MHz | ✅ Works |
| 3DS XL | 64MB | 268MHz | ✅ Works |
| 2DS | 64MB | 268MHz | ✅ Works |
| New 3DS | 128MB | 804MHz | ⚡ Fast |
| New 3DS XL | 128MB | 804MHz | ⚡ Fast |
| New 2DS XL | 128MB | 804MHz | ⚡ Fast |

> **Tip**: For large archives (>50MB), New 3DS is recommended for better performance

### Performance Estimates
| Operation | 3DS Old | New 3DS |
|-----------|---------|---------|
| Download 10MB | ~30-60s | ~30-60s |
| Extract ZIP 10MB | ~10-20s | ~3-7s |
| Extract TAR.GZ 10MB | ~15-30s | ~5-10s |
| Extract 7Z 10MB | ~20-40s | ~7-15s |

---

## Building from Source

### Prerequisites
```bash
# Install devkitPRO and dependencies
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-citro3d 3ds-citro2d \
                   3ds-libarchive 3ds-zlib 3ds-bzip2 3ds-xz 3ds-zstd
```

### Build

```bash
git clone https://github.com/Marcogn/3ds-zip-extractor.git
cd 3ds-zip-extractor
make
```

**Output**: `3ds-zip-extractor.3dsx`

---

## Documentation

📚 **Complete Guides**:
- [User Guide](docs/USER_GUIDE.md) - Detailed usage instructions
- [Technical Documentation](docs/TECHNICAL.md) - Architecture and API
- [Developer Guide](docs/DEVELOPMENT.md) - Contributing notes

📖 **Other**:
- [CONTRIBUTING.md](CONTRIBUTING.md) - How to contribute
- [SECURITY.md](SECURITY.md) - Security policy
- [LICENSE](LICENSE) - MIT License

---

## Troubleshooting

### Download fails
- ✅ Check your 3DS WiFi connection
- ✅ Test the URL in a browser first
- ✅ Enable `auto_retry=true` in config

### Extraction fails

- ✅ Check available space on SD card
- ✅ Verify the format is supported
- ✅ File might be corrupted, try re-downloading

### App crashes/freezes

- ✅ Archive might be too large for Old 3DS
- ✅ Close other apps to free RAM
- ✅ Try on New 3DS if available

### Format not supported

- ✅ Check the [format list](#supported-formats)
- ✅ LZ4 files are not supported (re-compress to .gz or .xz)

For other issues, consult the [User Guide](docs/USER_GUIDE.md) or open an [Issue](https://github.com/Marcogn/3ds-zip-extractor/issues).

---

## Google Drive Support

The app automatically converts Google Drive sharing URLs to direct download links:

**Supported URL formats**:
```
https://drive.google.com/file/d/FILE_ID/view
https://drive.google.com/open?id=FILE_ID
https://drive.google.com/uc?id=FILE_ID
```

**Features**:
- ✅ Automatic FILE_ID extraction
- ✅ Virus scan bypass for files >100MB
- ✅ Direct download link generation

**Example**:
```ini
# Simply paste the Google Drive share link
https://drive.google.com/file/d/1ABC123xyz/view

# The app converts it automatically to:
https://drive.google.com/uc?export=download&id=1ABC123xyz&confirm=t
```

---

## Technologies

Built with:
- **libarchive** - Multi-format archive extraction
- **libcurl** - HTTP/HTTPS downloads
- **mbedtls** - SSL/TLS support
- **zlib, libbz2, liblzma, libzstd** - Compression algorithms
- **citro2d/citro3d** - GPU rendering
- **libctru** - Nintendo 3DS system library

---

## How It Works

### 1. Download Phase
```
User provides URL → libcurl downloads → Progress tracking → Temp file saved
```

### 2. Format Detection
```
Read file signature (magic bytes) → Identify format → Select appropriate decoder
```

### 3. Extraction Phase
```
libarchive opens archive → Extract files → Progress callback → Write to SD card
```

### 4. Cleanup
```
Remove temporary file → Show results → Ready for next file
```

---

## Queue Management

The app supports downloading and extracting multiple files in sequence:

1. Add multiple URLs to `config.txt`
2. Press **A** to start the queue
3. Monitor progress for each file
4. View queue status with **X**
5. Skip failed downloads with **Y**
6. Retry failed downloads after completion

**Queue States**:
- `[ ]` Pending
- `[>]` In progress
- `[✓]` Completed
- `[X]` Failed
- `[-]` Skipped

---

## FAQ

### Can I extract password-protected archives?
No, password-protected archives are not currently supported.

### What's the maximum file size?
Limited by available RAM and SD card space. On Old 3DS, very large archives (>200MB compressed) may cause issues.

### Does it work with Mega.nz or Dropbox?
Direct links from any service work. For Mega/Dropbox, you need to get a direct download URL (not a sharing page).

### Can I create archives, not just extract?
Currently, the app only supports extraction. Archive creation may be added in future versions.

### Why is my download slow?
3DS WiFi is limited to 802.11b/g (~54Mbps theoretical). Actual speed depends on your network and router.

---

## Contributing

Contributions are welcome! 🎉

1. **Fork** the repository
2. Create a **branch** for your feature
3. **Commit** your changes
4. Open a **Pull Request**

Read [CONTRIBUTING.md](CONTRIBUTING.md) for details.

---

## Changelog

### v2.0 (2026-02-02)
- ✨ Multi-format support (11+ formats)
- ✨ Automatic format detection
- ✨ Google Drive URL conversion
- ✨ libarchive integration
- ⚡ Improved performance on New 3DS
- 📚 Complete documentation rewrite

### v1.0
- Basic ZIP support
- HTTP/HTTPS downloads
- Console GUI

---

## License

Distributed under **MIT License**. See [LICENSE](LICENSE) for more information.

---

## Credits

- **devkitPRO team** - 3DS toolchain and libraries
- **libarchive team** - Excellent multi-format library
- **3DS homebrew community** - Support and testing

---

## Support the Project

If you find this project useful:
- ⭐ **Star** the repository
- 🐛 **Report** bugs via Issues
- 💡 **Suggest** features via Issues
- 🤝 **Contribute** code via Pull Requests
- 📢 **Share** with the 3DS community

---

## Contact

**Author**: Marcogn  
**Version**: 2.0 Multi-Format  
**Year**: 2026  
**Repository**: [github.com/Marcogn/3ds-zip-extractor](https://github.com/Marcogn/3ds-zip-extractor)

---

<p align="center">
  <strong>⭐ If you like this project, give it a star! ⭐</strong>
</p>

<p align="center">
  <sub>Made with ❤️ for the Nintendo 3DS homebrew community</sub>
</p>
