# 🎮 3DS Archive Extractor

<p align="center">
  <img src="icon.png" alt="3DS Archive Extractor Icon" width="128">
</p>

<p align="center">
  <strong>📦 Download and extract compressed archives directly on your Nintendo 3DS</strong><br>
  <em>✨ Supports 11+ archive formats with native GUI</em>
</p>

<p align="center">
  <a href="#-features">Features</a> •
  <a href="#-supported-formats">Formats</a> •
  <a href="#-installation">Installation</a> •
  <a href="#-usage">Usage</a> •
  <a href="#-documentation">Documentation</a>
</p>

<div align="center">

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Nintendo%203DS-red.svg)](https://www.nintendo.com/3ds/)
[![Version](https://img.shields.io/badge/Version-2.0.2-green.svg)](CHANGELOG.md)

</div>

---

## ✨ Features

| Feature | Description |
|---------|-------------|
| 📦 **Multi-format Support** | Extract ZIP, TAR, 7Z, RAR, GZIP, BZIP2, XZ and more |
| 📥 **Direct Download** | Download files from HTTP/HTTPS directly on your 3DS |
| 🔄 **Auto-retry** | Automatically retry failed downloads |
| 📂 **File Browser** | Choose where to extract files with built-in browser |
| 🎨 **Console Interface** | Stable text-based interface optimized for 3DS |
| ⚡ **Efficient** | Works on Old 3DS, blazing fast on New 3DS |
| ☁️ **Google Drive** | Automatic URL conversion for Google Drive links |
| 📋 **Queue Management** | Process multiple files in sequence |
| ✨ **Auto-Config** | Creates example config file on first run |

---

## 📦 Supported Formats

<table>
<thead>
<tr>
<th>Format</th>
<th>Extension</th>
<th>Compression</th>
<th>Status</th>
</tr>
</thead>
<tbody>
<tr><td>📁 <strong>ZIP</strong></td><td><code>.zip</code></td><td>DEFLATE/STORE</td><td>✅ Full</td></tr>
<tr><td>📦 <strong>TAR</strong></td><td><code>.tar</code></td><td>None</td><td>✅ Full</td></tr>
<tr><td>🗜️ <strong>TAR.GZ</strong></td><td><code>.tar.gz</code>, <code>.tgz</code></td><td>GZIP</td><td>✅ Full</td></tr>
<tr><td>🗜️ <strong>TAR.BZ2</strong></td><td><code>.tar.bz2</code>, <code>.tbz2</code></td><td>BZIP2</td><td>✅ Full</td></tr>
<tr><td>🗜️ <strong>TAR.XZ</strong></td><td><code>.tar.xz</code>, <code>.txz</code></td><td>LZMA2</td><td>✅ Full</td></tr>
<tr><td>🗜️ <strong>TAR.ZSTD</strong></td><td><code>.tar.zst</code></td><td>Zstandard</td><td>✅ Full</td></tr>
<tr><td>7️⃣ <strong>7-Zip</strong></td><td><code>.7z</code></td><td>LZMA</td><td>✅ Full</td></tr>
<tr><td>📚 <strong>RAR</strong></td><td><code>.rar</code></td><td>RAR</td><td>📖 Read-only</td></tr>
<tr><td>🗜️ <strong>GZIP</strong></td><td><code>.gz</code></td><td>DEFLATE</td><td>✅ Full</td></tr>
<tr><td>🗜️ <strong>BZIP2</strong></td><td><code>.bz2</code></td><td>Burrows-Wheeler</td><td>✅ Full</td></tr>
<tr><td>🗜️ <strong>XZ</strong></td><td><code>.xz</code></td><td>LZMA2</td><td>✅ Full</td></tr>
<tr><td>🗜️ <strong>ZSTD</strong></td><td><code>.zst</code></td><td>Zstandard</td><td>✅ Full</td></tr>
</tbody>
</table>

> ⚠️ **Note**: LZ4 format is not supported (library unavailable on 3DS platform)

---

## 📥 Installation

### 📋 Requirements

- 🎮 Nintendo 3DS / 2DS / New 3DS with **Homebrew Launcher**
- 📡 **WiFi** connection
- 💾 **SD card** with free space

### 🚀 Quick Start

1️⃣ **Download** the latest `3ds-zip-extractor.3dsx` from [Releases](https://github.com/Marcogn/3ds-zip-extractor/releases)

2️⃣ **Copy** the file to the `/3ds/` folder on your SD card:
```
SD:/
└── 3ds/
    └── 3ds-zip-extractor.3dsx
```

3️⃣ **Launch** the app from Homebrew Launcher 🎉

4️⃣ **First Run**: The app will automatically create a config file at:
```
SD:/3ds/zip-extractor/config.txt
```

5️⃣ **Edit** the `config.txt` file and add your download URLs

6️⃣ **Restart** the app to begin downloading!

> 💡 **Tip**: On first run, the app creates an example config with instructions. Just add your URLs!

> ✨ **New**: No need to manually create the config file - the app does it for you!

---

## 🎮 Usage

### First Time Setup

**The app automatically creates a config file on first run!**

Simply:
1. Launch the app (it will detect no config exists)
2. App creates `/3ds/zip-extractor/config.txt` with examples
3. Exit the app (press START)
4. Edit `config.txt` and add your URLs
5. Restart the app and press A to begin!

### Basic Configuration

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

### 🎮 Controls

<table>
<thead>
<tr>
<th>Button</th>
<th>Action</th>
<th>Context</th>
</tr>
</thead>
<tbody>
<tr><td><kbd>A</kbd></td><td>Start downloads/extraction</td><td>Main menu</td></tr>
<tr><td><kbd>B</kbd></td><td>Cancel current operation</td><td>During download</td></tr>
<tr><td><kbd>X</kbd></td><td>View download queue</td><td>Main menu</td></tr>
<tr><td><kbd>Y</kbd></td><td>Skip failed downloads</td><td>Queue view</td></tr>
<tr><td><kbd>SELECT</kbd></td><td>Open file browser</td><td>Main menu</td></tr>
<tr><td><kbd>START</kbd></td><td>Exit application</td><td>Anytime</td></tr>
<tr><td><kbd>L</kbd> / <kbd>R</kbd></td><td>Navigate pages</td><td>Queue view</td></tr>
<tr><td><kbd>D-Pad</kbd></td><td>Navigate files</td><td>File browser</td></tr>
</tbody>
</table>

---

## 🏆 Compatibility

### 💻 Hardware

<table>
<thead>
<tr>
<th>Console</th>
<th>RAM</th>
<th>CPU</th>
<th>Performance</th>
<th>Status</th>
</tr>
</thead>
<tbody>
<tr><td>3DS</td><td>64MB</td><td>268MHz</td><td>🐢 Basic</td><td>✅ Works</td></tr>
<tr><td>3DS XL</td><td>64MB</td><td>268MHz</td><td>🐢 Basic</td><td>✅ Works</td></tr>
<tr><td>2DS</td><td>64MB</td><td>268MHz</td><td>🐢 Basic</td><td>✅ Works</td></tr>
<tr><td><strong>New 3DS</strong></td><td>128MB</td><td>804MHz</td><td>⚡ Fast</td><td>✅ Recommended</td></tr>
<tr><td><strong>New 3DS XL</strong></td><td>128MB</td><td>804MHz</td><td>⚡ Fast</td><td>✅ Recommended</td></tr>
<tr><td><strong>New 2DS XL</strong></td><td>128MB</td><td>804MHz</td><td>⚡ Fast</td><td>✅ Recommended</td></tr>
</tbody>
</table>

> 💡 **Tip**: For large archives (>50MB), New 3DS/XL is **strongly recommended** for optimal performance

### ⏱️ Performance Estimates

<table>
<thead>
<tr>
<th>Operation</th>
<th>3DS Old</th>
<th>New 3DS</th>
</tr>
</thead>
<tbody>
<tr><td>📥 Download 10MB</td><td>~30-60s</td><td>~30-60s</td></tr>
<tr><td>📦 Extract ZIP 10MB</td><td>~10-20s</td><td>~3-7s ⚡</td></tr>
<tr><td>🗜️ Extract TAR.GZ 10MB</td><td>~15-30s</td><td>~5-10s ⚡</td></tr>
<tr><td>7️⃣ Extract 7Z 10MB</td><td>~20-40s</td><td>~7-15s ⚡</td></tr>
</tbody>
</table>

> ℹ️ **Note**: Times vary based on compression ratio and file contents

---

## 🛠️ Building from Source

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

## 📚 Documentation

**Essential Documentation**:
- [User Guide](docs/USER_GUIDE.md) - Complete usage instructions and troubleshooting
- [Technical Documentation](docs/TECHNICAL.md) - Architecture, API, and format support details
- [Developer Guide](docs/DEVELOPMENT.md) - Build instructions and contribution guidelines
- [CHANGELOG.md](CHANGELOG.md) - Version history and migration guide

**Project Information**:
- [CONTRIBUTING.md](CONTRIBUTING.md) - How to contribute to the project
- [SECURITY.md](SECURITY.md) - Security policy and vulnerability reporting
- [LICENSE](LICENSE) - MIT License terms

---

## 🔧 Troubleshooting

### 📡 Download fails

<details>
<summary>Click to expand solutions</summary>

- ✅ Check your 3DS WiFi connection is active
- ✅ Test the URL in a browser first to verify it works
- ✅ Enable `auto_retry=true` in config.txt
- ✅ For Google Drive, make sure the file is publicly accessible
- ✅ Try a different DNS server (8.8.8.8 or 1.1.1.1)

</details>

### 📦 Extraction fails

<details>
<summary>Click to expand solutions</summary>

- ✅ Check available space on SD card (`df -h` on SD)
- ✅ Verify the archive format is supported (see [Supported Formats](#-supported-formats))
- ✅ File might be corrupted - try re-downloading
- ✅ Password-protected archives are NOT supported
- ✅ Try extracting to a different path

</details>

### 💥 App crashes/freezes

<details>
<summary>Click to expand solutions</summary>

- ✅ Archive might be too large for Old 3DS (>200MB compressed)
- ✅ Close other homebrew apps to free RAM
- ✅ Try on New 3DS/XL if available
- ✅ Verify you have the latest devkitPRO libraries
- ✅ Check if the .3dsx file is not corrupted
- ✅ Make sure you have enough space on SD card

</details>

### ❓ Icon shows as question mark

<details>
<summary>Click to expand solutions</summary>

- ✅ **This issue has been fixed in v2.0!**
- ✅ Re-download the latest .3dsx file
- ✅ The icon.png file must be 48x48 RGBA format
- ✅ The SMDH metadata is now properly embedded

</details>

### 🚫 Format not supported

<details>
<summary>Click to expand solutions</summary>

- ✅ Check the [supported formats list](#-supported-formats)
- ✅ LZ4 files are NOT supported (re-compress to .gz or .xz)
- ✅ Encrypted/password-protected archives are NOT supported
- ✅ Some exotic RAR versions might not work (use RAR5)

</details>

> 📚 For detailed troubleshooting, consult the [User Guide](docs/USER_GUIDE.md) or open an [Issue](https://github.com/Marcogn/3ds-zip-extractor/issues)

---

## ☁️ Google Drive Support

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

### v2.0.2 (2026-02-02)
- ✨ **Auto-config creation**: App creates example config on first run
- 🐛 **Data abort fix**: Fixed stack overflow issues
- 💪 **Stability**: Heap allocation for large structures
- 📝 **Better UX**: Clear messages and instructions
- 🔧 **Icon fix**: Proper 48x48 RGBA icon included

### v2.0.1 (2026-02-02)
- 🐛 Fixed data abort crashes on real hardware
- 🔧 GUI disabled for stability (console-only mode)
- 💾 Heap allocation to prevent stack overflow
- ✅ Stable on Old 3DS and New 3DS

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
