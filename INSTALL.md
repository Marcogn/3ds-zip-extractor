# Installation and Setup Guide

## Prerequisites

### Installing devkitPro

1. Follow the official installation guide at: https://devkitpro.org/wiki/Getting_Started

2. Install the 3DS development tools:
```bash
# On Linux/macOS
sudo dkp-pacman -S 3ds-dev

# On Windows (in MSYS2)
pacman -S 3ds-dev
```

3. Install required libraries:
```bash
sudo dkp-pacman -S 3ds-curl 3ds-libarchive 3ds-mbedtls 3ds-zlib
```

### Setting Environment Variables

Make sure these environment variables are set:

```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=$DEVKITPRO/devkitARM
export PATH=$DEVKITARM/bin:$PATH
```

Add these to your `~/.bashrc` or `~/.zshrc` to make them permanent.

## Quick Start

1. **Clone and build:**
```bash
git clone https://github.com/Marcogn/3ds-zip-extractor.git
cd 3ds-zip-extractor
make
```

2. **Prepare SD card:**
```bash
# Copy the application
cp 3ds-zip-extractor.3dsx /path/to/sd/card/3ds/

# Copy the config file
mkdir -p /path/to/sd/card/3ds/zip-extractor/
cp config.txt /path/to/sd/card/3ds/zip-extractor/
```

3. **Configure settings and URLs:**
Edit `/3ds/zip-extractor/config.txt` on your SD card:

4. **Run on 3DS:**
Launch the application from Homebrew Launcher.

## Configuration Options

### URLs Configuration File

The application reads URLs from: `sdmc:/3ds/zip-extractor/urls.txt`

Format:
```
# Comment lines start with #
https://example.com/file1.zip
https://example.com/file2.tar.gz
https://drive.google.com/file/d/FILE_ID/view
```

### Custom Extract Path

Edit `source/main.c` line 12:
```c
#define DEFAULT_EXTRACT_PATH "sdmc:/your/path/"
```

### Buffer Size

For different performance characteristics, modify line 11:
```c
#define DOWNLOAD_BUFFER_SIZE (128 * 1024)  // Change size here
```

### Temporary Download Location

Change line 13:
```c
#define TEMP_DOWNLOAD_PATH "sdmc:/your/temp/path.tmp"
```

## Testing

### Testing with Local Server

For development, you can test with a local HTTP server:

1. Start a local server:
```bash
python3 -m http.server 8000
```

2. Find your local IP:
```bash
ip addr show | grep "inet "
```

3. Use in the app:
```c
const char* download_url = "http://192.168.1.100:8000/test.zip";
```

### Testing Google Drive Links

1. Upload a file to Google Drive
2. Get the share link
3. Use it directly in the application

Example formats that work:
- `https://drive.google.com/file/d/1ABC...XYZ/view`
- `https://drive.google.com/open?id=1ABC...XYZ`

## Common Build Issues

### "DEVKITARM not set"
Make sure environment variables are properly set:
```bash
echo $DEVKITARM  # Should show /opt/devkitpro/devkitARM
```

### "curl.h not found"
Install 3ds-curl:
```bash
sudo dkp-pacman -S 3ds-curl
```

### "archive.h not found"
Install 3ds-libarchive:
```bash
sudo dkp-pacman -S 3ds-libarchive
```

### "mbedtls not found"
Install 3ds-mbedtls:
```bash
sudo dkp-pacman -S 3ds-mbedtls
```

## Advanced Usage

### Using Command Line Arguments (Future Enhancement)

Currently, the URL must be hardcoded. To accept command line arguments, you would need to:

1. Parse `argc` and `argv` in `main()`
2. Validate the URL format
3. Use the provided URL instead of the hardcoded one

Example implementation:
```c
if (argc > 1) {
    download_url = argv[1];
}
```

### Integration with Configuration File

You could implement a config file reader:

1. Create `sdmc:/3ds/zip-extractor/config.txt`
2. Read URL from file at startup
3. Parse and use the URL

## Performance Tips

1. **Network Speed**: Download speed depends on your 3DS WiFi connection and the server
2. **SD Card Speed**: Extraction speed depends on your SD card's write speed
3. **Large Files**: For files over 100MB, ensure you have stable WiFi and sufficient battery

## Debugging

### Enable Verbose Logging

Add debug printf statements in key locations:
```c
printf("Debug: Starting download from %s\n", converted_url);
```

### Check Network Status

Add this before download:
```c
u32 wifi_status = 0;
ACU_GetWifiStatus(&wifi_status);
printf("WiFi Status: %lu\n", wifi_status);
```

### Monitor Memory Usage

```c
printf("Free memory: %lu KB\n", osGetMemRegionFree(MEMREGION_APPLICATION) / 1024);
```

## Deployment Options

### Homebrew Launcher (.3dsx)
- Build with `make`
- Copy to SD card `/3ds/` folder
- Launch from Homebrew Launcher

### CIA Installation (Advanced)
Requires additional tools:
1. Use `makerom` to create a CIA
2. Install with FBI or other CIA installer
3. Launch from HOME Menu

## Support

For issues and questions:
- Open an issue on GitHub
- Check existing issues for solutions
- Refer to devkitPro documentation

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

Focus areas for contribution:
- URL input interface
- Configuration file support
- Better error messages
- More archive format support
- Download queue system
