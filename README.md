# 3DS Zip Extractor

<p align="center">
  <img src="icon.png" alt="3DS Zip Extractor Icon" width="96" height="96">
</p>

<p align="center">
  <strong>A homebrew application for New Nintendo 3DS that downloads and extracts compressed files from web URLs</strong>
</p>

<p align="center">
  <a href="https://github.com/Marcogn/3ds-zip-extractor/releases"><img src="https://img.shields.io/github/v/release/Marcogn/3ds-zip-extractor?style=flat-square" alt="Release"></a>
  <a href="https://github.com/Marcogn/3ds-zip-extractor/blob/main/LICENSE"><img src="https://img.shields.io/github/license/Marcogn/3ds-zip-extractor?style=flat-square" alt="License"></a>
  <a href="https://github.com/Marcogn/3ds-zip-extractor/issues"><img src="https://img.shields.io/github/issues/Marcogn/3ds-zip-extractor?style=flat-square" alt="Issues"></a>
</p>

---

> **📖 New to this project? Start with [QUICKSTART.md](QUICKSTART.md) for step-by-step instructions in Italian and English!**

## ⚠️ Important: How to Use This

1. **On your PC**: Install devkitPro and compile this project (see [QUICKSTART.md](QUICKSTART.md))
2. **Copy to SD**: Put the compiled `.3dsx` file and `urls.txt` on your 3DS SD card
3. **On your 3DS**: Run from Homebrew Launcher

**You cannot compile directly on the 3DS.** You need a PC with devkitPro installed.

## Features

- **Hybrid GUI**: Console text + graphical progress bars for better visual feedback
- **Download from URLs**: Download compressed files from any HTTP/HTTPS URL
- **Multiple Downloads**: Support for downloading and extracting multiple files sequentially
- **Configuration File**: Enhanced config file with both settings and URLs
- **Download Queue Management**: View queue status, skip failed items, retry downloads
- **File Browser**: Browse and select extraction destination folder
- **Auto-Retry**: Automatically retry failed downloads with configurable attempts
- **🌟 Enhanced Google Drive Support**: 
  - Automatically converts Google Drive URLs to direct download links
  - Support for multiple URL formats (`/file/d/`, `/open?id=`, `/uc?id=`)
  - **Automatic virus scan bypass** for files >100MB using `confirm=t` parameter
  - Warning for unsupported folder URLs
- **🌟 Extended Archive Format Support**:
  - **ZIP** (.zip) - with all compression methods (deflate, bzip2, lzma, etc.)
  - **7-Zip** (.7z) - full support
  - **TAR** (.tar, .tar.gz, .tar.bz2, .tar.xz) - all variants
  - **RAR** (.rar) - read support
  - And many more via libarchive
- **Resume Support**: Automatically resumes interrupted downloads
- **Large File Handling**: Efficient streaming for large files
- **Progress Tracking**: Real-time download and extraction progress display with graphical bars
- **Batch Summary**: Shows statistics after processing all files
- **Configurable Output**: Custom extract path via configuration file or file browser

## Requirements

### To Build (on PC):
- devkitARM toolchain
- libctru
- citro3d and citro2d (for hybrid GUI)
- libcurl (from devkitPro portlibs)
- libarchive (from devkitPro portlibs)
- mbedtls (from devkitPro portlibs)
- **Additional compression libraries required by libarchive:**
  - libzstd (3ds-libzstd)
  - liblzma (3ds-liblzma) 
  - libbz2 (3ds-bzip2)

**⚠️ IMPORTANT NOTE:** The current version of libarchive in devkitPro portlibs has linking issues with compression libraries. The build may fail with "undefined reference" errors for ZSTD, LZMA, and BZ2 functions even after installing the required packages. This is a known limitation of the current devkitPro 3DS toolchain.

**See [QUICKSTART.md](QUICKSTART.md) for installation instructions.**

### To Run (on 3DS):
- New Nintendo 3DS with custom firmware (CFW)
- Homebrew Launcher or CIA installer
- Internet connection

## Building

**Full instructions in [QUICKSTART.md](QUICKSTART.md)**

Quick version:

1. Install devkitPro and 3DS development tools
```bash
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-libarchive 3ds-mbedtls 3ds-citro3d 3ds-citro2d
```

2. Clone and build:
```bash
git clone https://github.com/Marcogn/3ds-zip-extractor.git
cd 3ds-zip-extractor
make
```

The output will be `3ds-zip-extractor.3dsx`

## Usage

### Setting Up Configuration

1. Copy `config.txt` to your SD card at: `sdmc:/3ds/zip-extractor/config.txt`

2. Edit the file to configure settings and add your URLs:

```
# Settings
extract_path=sdmc:/extracted/
auto_retry=true
max_retries=3

# URLs (one per line)
https://example.com/file1.zip
https://example.com/file2.tar.gz
https://drive.google.com/file/d/FILE_ID/view
```

### Configuration Options

**Settings:**
- `extract_path=<path>` - Where to extract files (default: `sdmc:/extracted/`)
- `auto_retry=true/false` - Automatically retry failed downloads (default: false)
- `max_retries=<0-10>` - Maximum retry attempts (default: 3)

**URLs:**
- Add one URL per line after the settings
- Lines starting with `#` are comments
- Empty lines are ignored

### Supported URL Formats

**Direct URLs:**
```
https://example.com/archive.zip
http://server.com/file.tar.gz
```

**Google Drive URLs:**
- `https://drive.google.com/file/d/FILE_ID/view`
- `https://drive.google.com/open?id=FILE_ID`

### Configuring Extract Path

By default, files are extracted to `sdmc:/extracted/`. To change this, modify the `DEFAULT_EXTRACT_PATH` constant in `source/main.c`:

```c
#define DEFAULT_EXTRACT_PATH "sdmc:/your/custom/path/"
```

### Running the Application

1. Build the application: `make`
2. Copy `3ds-zip-extractor.3dsx` to your SD card's `/3ds/` folder
3. Copy `config.txt` to `/3ds/zip-extractor/config.txt` on your SD card
4. Edit `config.txt` to configure settings and add your download URLs
5. Launch via Homebrew Launcher
6. Press **SELECT** to browse and choose extraction folder
7. Press **X** to view download queue with status indicators
8. Press **A** to start downloading and extracting all files
9. Press **Y** (in queue view) to skip all failed downloads
10. Press **L/R** (in queue view) to navigate pages
11. Press **B** to cancel during download or go back in menus
12. Press **A** (after completion) to retry failed downloads
13. Press **START** to exit

### Controls

**Main Menu:**
- **A** - Start downloads
- **X** - View queue
- **SELECT** - Open file browser
- **START** - Exit

**File Browser:**
- **D-Pad ↑↓** - Navigate
- **A** - Enter folder
- **Y** - Select current folder as destination
- **B** - Cancel/Back

**Queue View:**
- **L/R** - Change page
- **Y** - Skip failed
- **A** - Continue/Retry
- **B** - Back

### Queue Status Indicators

- `[ ]` - Pending (not yet downloaded)
- `[>]` - In Progress (currently downloading)
- `[✓]` - Completed (successfully extracted)
- `[X]` - Failed (download or extraction error)
- `[-]` - Skipped (manually skipped)

## Features Detail

### Enhanced Configuration File

The `config.txt` file now supports both settings and URLs in a single file:

**Settings Section:**
```
extract_path=sdmc:/my-games/
auto_retry=true
max_retries=3
```

**URLs Section:**
```
https://example.com/file1.zip
https://example.com/file2.tar.gz
```

Settings must come before URLs. All settings are optional and have defaults.

### Download Queue Management

The application features a comprehensive queue management system:

**View Queue Status:**
- Press X to see all downloads with their current status
- Navigate multiple pages with L/R buttons
- See pending, completed, failed, and skipped items

**Manage Failed Downloads:**
- Press Y to skip all failed downloads
- Press A after completion to retry only failed items
- Auto-retry option retries failed downloads automatically

**Resume Capability:**
- Queue state persists through the session
- Completed items won't be re-downloaded
- Failed items can be retried without re-downloading successful ones

### Multiple File Downloads

The application can process multiple URLs sequentially. Simply add each URL on a new line in the `urls.txt` file. The application will:
1. Download each file in order
2. Extract each archive after download
3. Show progress for each file
4. Display a summary at the end with success/failure counts

### Configuration File Format

The `urls.txt` file supports:
- One URL per line
- Comments (lines starting with #)
- Empty lines (ignored)
- Up to 50 URLs

Example:
```
# My downloads
https://example.com/file1.zip
https://example.com/file2.tar.gz

# More files
https://example.com/file3.7z
```

### Download Resumption

If a download is interrupted (e.g., connection lost), the application will automatically resume from where it left off on the next attempt. The partial file is stored at `sdmc:/temp_download.tmp`.

### Large File Support

The application uses streaming for both download and extraction, meaning it doesn't need to load entire files into memory. This allows handling files larger than available RAM.

### Google Drive Support

Google Drive URLs are automatically detected and converted to direct download links. 

#### Supported URL Formats:
- `https://drive.google.com/file/d/FILE_ID/view`
- `https://drive.google.com/file/d/FILE_ID/edit`  
- `https://drive.google.com/open?id=FILE_ID`
- `https://drive.google.com/uc?id=FILE_ID`

#### Large File Support (>100MB):
For files larger than 100MB, Google Drive shows a virus scan warning page. The application automatically adds the `confirm=t` parameter to bypass this warning and download the file directly.

**Example:**
```
Original: https://drive.google.com/file/d/1ABC...XYZ/view
Converted: https://drive.google.com/uc?export=download&id=1ABC...XYZ&confirm=t
```

#### Limitations:
- Folder URLs are **not supported** (only individual files)
- Files must be publicly accessible or have "Anyone with the link" permission
- Very large files (>multiple GB) may timeout depending on your connection

### Supported Archive Formats

Via libarchive, the following formats are supported:

#### Fully Supported:
- **ZIP** (.zip) - All compression methods:
  - Store (uncompressed)
  - Deflate (most common)
  - BZIP2
  - LZMA
  - PPMD
- **7-Zip** (.7z) - Full support with all compression methods
- **TAR** (.tar) - Uncompressed and compressed variants:
  - .tar.gz (gzip)
  - .tar.bz2 (bzip2)
  - .tar.xz (lzma)
- **GZIP** (.gz) - Single file compression
- **BZIP2** (.bz2) - Single file compression

#### Read-Only Support:
- **RAR** (.rar) - Extraction only (no creation)
- **ISO** (.iso) - ISO9660 format

#### Note on Compression Algorithms:
The application supports various compression algorithms through libarchive:
- **DEFLATE**: Standard, fast, good compression
- **BZIP2**: Better compression, slower
- **LZMA/XZ**: Best compression, slowest
- **ZSTD**: Modern, fast, good compression

**⚠️ Current Limitation**: Due to devkitPro toolchain issues, advanced compression support (BZIP2, LZMA, ZSTD) is currently unavailable. See [COMPILATION_ISSUES.md](COMPILATION_ISSUES.md) for details.

## Troubleshooting

### "socInit failed"
Network initialization failed. Make sure you're running on a 3DS with CFW and network access.

### "Download failed"
Check your internet connection and verify the URL is correct. For Google Drive, make sure the file is publicly accessible or the share link is correct.

### "Failed to open archive"
The downloaded file may be corrupted or not a valid archive format. Try downloading again or verify the file format.

### "Extraction failed"
Check that you have enough free space on your SD card and that the archive is not corrupted.

## Security Notes

### SSL/TLS Certificate Verification

The application disables SSL certificate verification for HTTPS connections. This is a common practice for 3DS homebrew due to the platform's limitations with certificate stores. However, this means:

- Connections may be vulnerable to man-in-the-middle attacks
- For security-sensitive downloads, verify file integrity after download
- Consider using trusted networks when downloading sensitive files

This trade-off is necessary for broad HTTPS compatibility on the 3DS platform.

## Development

### Project Structure
```
3ds-zip-extractor/
├── source/          # Source code
│   └── main.c      # Main application code
├── include/        # Header files (if any)
├── data/           # Data files (if any)
├── gfx/            # Graphics files (if any)
├── Makefile        # Build configuration
└── README.md       # This file
```

### Extending the Application

To add URL input instead of hardcoding:
- Implement a keyboard input system using SwkbdState
- Integrate with the download flow

To add file selection:
- Use romfsInit() for bundled file lists
- Implement file browser UI

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on:
- How to report issues
- How to submit pull requests
- Code style guidelines
- Testing requirements

## Support

- **Documentation**: Check [QUICKSTART.md](QUICKSTART.md), [INSTALL.md](INSTALL.md), and [EXAMPLES.md](EXAMPLES.md)
- **Issues**: [GitHub Issues](https://github.com/Marcogn/3ds-zip-extractor/issues)
- **Security**: See [SECURITY.md](SECURITY.md) for security policy

## Acknowledgments

- **devkitPro Team** - For the excellent 3DS development toolchain
- **libctru** - 3DS system functions
- **libcurl** - HTTP/HTTPS downloads
- **libarchive** - Archive extraction support
- **mbedtls** - SSL/TLS implementation
- **citro3d/citro2d** - Graphics rendering

## Author

**Marcogn**
- GitHub: [@Marcogn](https://github.com/Marcogn)

## Disclaimer

This software is provided "as is", without warranty of any kind. Use at your own risk. Always backup your SD card before using homebrew applications.