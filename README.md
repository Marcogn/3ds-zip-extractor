# 3DS Zip Extractor

A homebrew application for New Nintendo 3DS that downloads and extracts compressed files from web URLs, including Google Drive.

> **📖 New to this project? Start with [QUICKSTART.md](QUICKSTART.md) for step-by-step instructions in Italian and English!**

## ⚠️ Important: How to Use This

1. **On your PC**: Install devkitPro and compile this project (see [QUICKSTART.md](QUICKSTART.md))
2. **Copy to SD**: Put the compiled `.3dsx` file and `urls.txt` on your 3DS SD card
3. **On your 3DS**: Run from Homebrew Launcher

**You cannot compile directly on the 3DS.** You need a PC with devkitPro installed.

## Features

- **Download from URLs**: Download compressed files from any HTTP/HTTPS URL
- **Multiple Downloads**: Support for downloading and extracting multiple files sequentially
- **Configuration File**: URLs are read from a text file (one per line)
- **Google Drive Support**: Automatically converts Google Drive URLs to direct download links
- **Multiple Archive Formats**: Supports ZIP, TAR, 7Z, and many other formats via libarchive
- **Resume Support**: Automatically resumes interrupted downloads
- **Large File Handling**: Efficient streaming for large files
- **Progress Tracking**: Real-time download and extraction progress display
- **Batch Summary**: Shows statistics after processing all files
- **Configurable Output**: Extract to default or custom directory

## Requirements

### To Build (on PC):
- devkitARM toolchain
- libctru
- citro3d
- libcurl (from devkitPro portlibs)
- libarchive (from devkitPro portlibs)
- mbedtls (from devkitPro portlibs)

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
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-libarchive 3ds-mbedtls
```

2. Clone and build:
```bash
git clone https://github.com/Marcogn/3ds-zip-extractor.git
cd 3ds-zip-extractor
make
```

The output will be `3ds-zip-extractor.3dsx`

## Usage

### Setting Up URLs

1. Copy `urls.txt` to your SD card at: `sdmc:/3ds/zip-extractor/urls.txt`

2. Edit the file and add your download URLs (one per line):

```
# Comments start with #
https://example.com/file1.zip
https://example.com/file2.tar.gz
https://drive.google.com/file/d/FILE_ID/view
```

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
3. Copy `urls.txt` to `/3ds/zip-extractor/urls.txt` on your SD card
4. Edit `urls.txt` to add your download URLs
5. Launch via Homebrew Launcher
6. Press **X** to view configured URLs
7. Press **A** to start downloading and extracting all files
8. Press **B** to cancel during download or extraction
9. Press **START** to exit

## Features Detail

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

Google Drive URLs are automatically detected and converted to direct download links. Supported formats:
- `https://drive.google.com/file/d/FILE_ID/view`
- `https://drive.google.com/open?id=FILE_ID`

### Supported Archive Formats

Via libarchive, the following formats are supported:
- ZIP (.zip)
- TAR (.tar, .tar.gz, .tar.bz2, .tar.xz)
- 7-Zip (.7z)
- RAR (.rar)
- And many more...

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

This project is provided as-is for educational and personal use.

## Credits

- Uses libctru for 3DS system functions
- Uses libcurl for HTTP downloads
- Uses libarchive for archive extraction
- Uses mbedtls for SSL/TLS support

## Author

Marcogn