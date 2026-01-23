# 3DS Zip Extractor

A homebrew application for New Nintendo 3DS that downloads and extracts compressed files from web URLs, including Google Drive.

## Features

- **Download from URLs**: Download compressed files from any HTTP/HTTPS URL
- **Google Drive Support**: Automatically converts Google Drive URLs to direct download links
- **Multiple Archive Formats**: Supports ZIP, TAR, 7Z, and many other formats via libarchive
- **Resume Support**: Automatically resumes interrupted downloads
- **Large File Handling**: Efficient streaming for large files
- **Progress Tracking**: Real-time download and extraction progress display
- **Configurable Output**: Extract to default or custom directory

## Requirements

### To Build:
- devkitARM
- libctru
- citro3d
- libcurl (from devkitPro portlibs)
- libarchive (from devkitPro portlibs)
- mbedtls (from devkitPro portlibs)

### To Run:
- New Nintendo 3DS with custom firmware
- Homebrew Launcher or CIA installer

## Building

1. Install devkitPro and required libraries:
```bash
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-libarchive 3ds-mbedtls
```

2. Clone the repository:
```bash
git clone https://github.com/Marcogn/3ds-zip-extractor.git
cd 3ds-zip-extractor
```

3. Edit `source/main.c` to set your download URL (see Usage section)

4. Build:
```bash
make
```

The output will be `3ds-zip-extractor.3dsx`

## Usage

### Setting the Download URL

Before building, edit `source/main.c` and change the `download_url` variable on line 337:

```c
// Example URL - user should modify this
const char* download_url = "http://example.com/file.zip";
```

You can use:
- Direct HTTP/HTTPS URLs: `http://example.com/file.zip`
- Google Drive share links: `https://drive.google.com/file/d/FILE_ID/view`
- Google Drive open links: `https://drive.google.com/open?id=FILE_ID`

### Configuring Extract Path

By default, files are extracted to `sdmc:/extracted/`. To change this, modify the `DEFAULT_EXTRACT_PATH` constant in `source/main.c`:

```c
#define DEFAULT_EXTRACT_PATH "sdmc:/your/custom/path/"
```

### Running the Application

1. Copy `3ds-zip-extractor.3dsx` to your SD card's `/3ds/` folder
2. Launch via Homebrew Launcher
3. Press **A** to start download and extraction
4. Press **B** to cancel during download or extraction
5. Press **START** to exit

## Features Detail

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