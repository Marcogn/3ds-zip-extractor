# 3DS Archive Extractor v1.0

**First Stable Release** - February 5, 2026

Download and extract compressed archives directly on your Nintendo 3DS console.

---

## 🎉 What's New

This is the first stable release of 3DS Archive Extractor, bringing a complete solution for downloading and extracting archives on Nintendo 3DS hardware.

### Key Features

- **Multi-Format Support**: Extract ZIP, TAR, 7Z, RAR, and many more archive formats
- **Network Downloads**: Download files via HTTP/HTTPS directly to your 3DS
- **Google Drive Integration**: Automatic URL conversion for Google Drive shared links
- **Modern GUI**: Dark blue themed interface using citro2d/citro3d GPU acceleration
- **Background Downloads**: Continue downloads in sleep mode (press HOME button)
- **LED Notifications**: Visual feedback when operations complete
- **Auto-Configuration**: Automatic config file creation on first run
- **File Browser**: Built-in browser to select extraction directories
- **Download Queue**: Process multiple files sequentially
- **Auto-Retry**: Automatically retry failed downloads

---

## 📦 Supported Archive Formats

| Format | File Extensions | Notes |
|--------|----------------|-------|
| **ZIP** | .zip | Full support (DEFLATE/STORE) |
| **TAR** | .tar, .tar.gz, .tar.bz2, .tar.xz, .tar.zst | All compression methods |
| **7-Zip** | .7z | Full support |
| **RAR** | .rar | Read-only (extraction only) |
| **GZIP** | .gz | Single file compression |
| **BZIP2** | .bz2 | Single file compression |
| **XZ** | .xz | LZMA2 compression |
| **ZSTD** | .zst | Zstandard compression |

**Total: 11+ archive format variants supported**

---

## 🎮 Installation

### Requirements
- Nintendo 3DS / 2DS / New 3DS with Custom Firmware
- Homebrew Launcher installed
- WiFi connection (for downloads)
- SD card with free space

### Steps
1. Download `3ds-zip-extractor.3dsx` from this release
2. Copy the file to the `/3ds/` folder on your SD card
3. Insert SD card into your 3DS
4. Launch Homebrew Launcher
5. Select "3DS Zip Extractor" from the menu

---

## 🚀 Quick Start Guide

### First Run
When you launch the app for the first time:
1. It will automatically create a config file at `/3ds/zip-extractor/config.txt`
2. You'll see a message: "Config File Created!"
3. Press START to exit

### Configure URLs
1. Remove SD card and open it on your computer
2. Edit `/3ds/zip-extractor/config.txt` in a text editor
3. Add your download URLs (one per line):
   ```ini
   extract_path=sdmc:/extracted/
   auto_retry=true
   max_retries=3
   
   # Add your URLs below
   https://example.com/file.zip
   https://drive.google.com/file/d/YOUR_FILE_ID/view
   ```
4. Save the file and eject SD card safely

### Download & Extract
1. Insert SD card back into 3DS
2. Launch the app again
3. You'll see "Loaded X URL(s) from config"
4. Press **A** to start downloads
5. Progress bar shows download status
6. Archives are automatically extracted after download
7. Press START to exit when done

---

## 🎛️ Controls

| Button | Action |
|--------|--------|
| **A** | Start downloads / Confirm selection |
| **B** | Cancel current operation |
| **X** | View download queue status |
| **Y** | Skip failed downloads |
| **SELECT** | Open file browser |
| **START** | Exit application |
| **D-Pad** | Navigate menus / file browser |
| **L/R** | Switch pages (in queue view) |

---

## 💡 Features in Detail

### Google Drive Support
Simply paste Google Drive sharing URLs - they're automatically converted to direct download links:
```
https://drive.google.com/file/d/1ABC123xyz/view
↓ Automatically converted ↓
https://drive.google.com/uc?id=1ABC123xyz&export=download
```

### Sleep Mode Downloads
Press the HOME button during downloads - the 3DS enters sleep mode but downloads continue in the background. The LED notification will alert you when complete.

### LED Notifications
- 🟢 **Green LED**: Download + Extraction completed successfully
- 🔴 **Pink/Red LED**: Download completed, extraction in progress

### Auto-Retry
If a download fails due to network issues, the app automatically retries (up to 3 times by default). Configure in `config.txt`:
```ini
auto_retry=true
max_retries=5
```

---

## 🖥️ Hardware Compatibility

### Old Nintendo 3DS / 3DS XL / 2DS
- **CPU**: 268 MHz ARM11
- **RAM**: 64 MB
- **Performance**: Good for most archives
- **Recommended**: Files under 50 MB

### New Nintendo 3DS / New 3DS XL / New 2DS XL
- **CPU**: 804 MHz ARM11
- **RAM**: 128 MB
- **Performance**: Excellent for all archives
- **Recommended**: All file sizes

**Note**: Archives larger than 200 MB may cause memory issues on Old 3DS hardware.

---

## 🔧 Building from Source

### Prerequisites
```bash
# Install devkitPRO toolchain
sudo dkp-pacman -S 3ds-dev

# Install required libraries
sudo dkp-pacman -S 3ds-curl 3ds-libarchive 3ds-zlib \
                   3ds-bzip2 3ds-xz 3ds-zstd \
                   3ds-citro2d 3ds-citro3d
```

### Compile
```bash
git clone https://github.com/Marcogn/3ds-zip-extractor.git
cd 3ds-zip-extractor
make
```

Output: `3ds-zip-extractor.3dsx`

---

## ❗ Troubleshooting

### Download fails with network error
- Check WiFi connection is active
- Verify URL is accessible from a browser
- Enable `auto_retry=true` in config.txt
- For Google Drive: ensure file is publicly accessible

### Extraction fails
- Check available SD card space
- Verify archive is not corrupted (re-download)
- Password-protected archives are not supported
- Try archive on a computer first to verify it's valid

### App freezes or crashes
- Archive may be too large for available RAM
- Close other homebrew apps to free memory
- Try on New 3DS if using Old 3DS
- Check SD card is not full

### Config file not found
- Ensure `/3ds/zip-extractor/` directory exists
- Check SD card is not write-protected
- Manually create config.txt if auto-creation fails

---

## 🐛 Known Limitations

- **Password Protection**: Password-protected archives are not supported
- **LZ4 Format**: Not supported (library unavailable for 3DS)
- **Large Archives**: Files over 200 MB may fail on Old 3DS
- **Mega/Dropbox**: Direct download URLs only (not web sharing pages)

---

## 📝 Technical Details

### Built With
- **citro2d/citro3d**: GPU-accelerated graphics rendering
- **libarchive**: Multi-format archive extraction engine
- **libcurl**: HTTP/HTTPS network transfers
- **mbedtls**: SSL/TLS encryption support
- **zlib, libbz2, liblzma, libzstd**: Compression libraries
- **libctru**: Nintendo 3DS system library

### Performance
- **Old 3DS**: ~500 KB/s download, ~2 MB/s extraction
- **New 3DS**: ~1.5 MB/s download, ~8 MB/s extraction

### Storage
- **App Size**: 1.5 MB
- **RAM Usage**: ~20-30 MB during operation
- **SD Card**: Extraction space depends on archive size

---

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

---

## 📜 License

This project is licensed under the MIT License.

Copyright (c) 2026 Marcogn

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.

---

## 🙏 Credits

- **devkitPro Team**: 3DS toolchain and system libraries
- **libarchive Project**: Multi-format archive library
- **3DS Homebrew Community**: Documentation and support

---

## 🔗 Links

- **Repository**: https://github.com/Marcogn/3ds-zip-extractor
- **Issues**: https://github.com/Marcogn/3ds-zip-extractor/issues
- **Releases**: https://github.com/Marcogn/3ds-zip-extractor/releases

---

## 📊 Release Stats

- **Version**: 1.0
- **Release Date**: February 5, 2026
- **Total Downloads**: TBD
- **Supported Formats**: 11+
- **Lines of Code**: ~2,500
- **Development Time**: February 2026

---

**Enjoy downloading and extracting archives on your Nintendo 3DS!**

If you encounter any issues, please report them on the [Issues page](https://github.com/Marcogn/3ds-zip-extractor/issues).
