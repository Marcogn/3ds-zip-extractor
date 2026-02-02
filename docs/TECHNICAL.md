# Technical Documentation - 3DS Archive Extractor

## Overview

3DS Archive Extractor is a homebrew application for Nintendo 3DS that allows you to download and extract compressed archives in multiple formats directly on the console.

---

## Supported Formats

The application supports **11+ archive formats** via libarchive:

### Complete Archives
- **ZIP** (.zip) - Standard format, full support
- **TAR** (.tar) - Tape archives, uncompressed
- **7-Zip** (.7z) - High compression
- **RAR** (.rar) - Read-only (extraction)

### Compressed TAR Archives
- **TAR.GZ** (.tar.gz, .tgz) - GZIP compression
- **TAR.BZ2** (.tar.bz2, .tbz2) - BZIP2 compression
- **TAR.XZ** (.tar.xz, .txz) - XZ/LZMA compression
- **TAR.ZSTD** (.tar.zst) - Zstandard compression

### Single Compressed Files
- **GZIP** (.gz) - DEFLATE compression
- **BZIP2** (.bz2) - Burrows-Wheeler compression
- **XZ** (.xz) - LZMA2 compression
- **ZSTD** (.zst) - Zstandard compression

### Not Supported
- **LZ4** (.lz4) - Library not available on 3DS portlibs

---

## Architecture

```
┌─────────────────────────────────────────┐
│    3DS Archive Extractor (main.c)      │
│  - Download Manager (libcurl)          │
│  - Queue System                         │
│  - GUI (citro2d/citro3d)               │
├─────────────────────────────────────────┤
│   Archive Extractor Module              │
│   (archive_extractor.c)                 │
│  - Format Detection (magic bytes)       │
│  - Multi-format Extraction              │
├─────────────────────────────────────────┤
│   Libraries                             │
│  ├─ libarchive (multi-format)          │
│  ├─ libcurl (HTTP/HTTPS)               │
│  ├─ zlib (gzip/deflate)                │
│  ├─ libbz2 (bzip2)                     │
│  ├─ liblzma (xz/lzma)                  │
│  ├─ libzstd (zstandard)                │
│  └─ mbedtls (SSL/TLS)                  │
├─────────────────────────────────────────┤
│   3DS System (libctru)                  │
└─────────────────────────────────────────┘
```

---

## Format Detection

Format detection occurs through analysis of "magic bytes" (file signature):

```c
// Signature examples
ZIP:    PK\x03\x04
GZIP:   0x1f 0x8b
BZIP2:  BZ
7Z:     37 7A BC AF 27 1C
RAR:    Rar!\x1a\x07
TAR:    "ustar" @ offset 257
XZ:     FD 37 7A 58 5A 00
ZSTD:   28 B5 2F FD
```

The file `archive_extractor.c` implements the function:
```c
ArchiveType detect_archive_type(const char* file_path);
```

---

## Operational Flow

1. **Initialization**
   - Reading `config.txt`
   - GUI initialization
   - Networking setup

2. **Download**
   - HTTP/HTTPS request via libcurl
   - Resume support for interrupted downloads
   - Automatic Google Drive URL conversion
   - Progress tracking

3. **Extraction**
   - Automatic format detection
   - Extraction via libarchive
   - Callback for progress updates
   - Robust error handling

4. **Cleanup**
   - Temporary file removal
   - Result reporting
   - Queue update

---

## Hardware Compatibility

### Supported Consoles
- Nintendo 3DS (Old) - 64MB RAM, 268MHz CPU
- Nintendo 3DS XL (Old)
- Nintendo 2DS
- New Nintendo 3DS - 128MB RAM, 804MHz CPU ⚡
- New Nintendo 3DS XL
- New Nintendo 2DS XL

### Memory Requirements
```
Download Buffer:      128KB
Extraction Buffer:    128KB
App Stack:            ~4MB
libarchive Heap:      2-8MB (dynamic)
────────────────────────────
Typical Total:        6-12MB

✅ Compatible with Old 3DS (64MB available)
✅ Optimal on New 3DS (128MB available)
```

### Performance

| Operation | Old 3DS (268MHz) | New 3DS (804MHz) |
|-----------|------------------|------------------|
| Download 10MB | ~30-60s | ~30-60s (I/O bound) |
| Extract ZIP 10MB | ~10-20s | ~3-7s |
| Extract TAR.GZ 10MB | ~15-30s | ~5-10s |
| Extract 7Z 10MB | ~20-40s | ~7-15s |

---

## Security

### Implemented Protections

1. **Buffer Overflow Protection**
   - Fixed and validated buffer sizes
   - Use of `strncpy` instead of `strcpy`
   - Array bounds checking

2. **Path Traversal Protection**
   - Flag `ARCHIVE_EXTRACT_SECURE_NODOTDOT`
   - Prevents extraction outside target directory
   - Path sanitization

3. **Memory Management**
   - No unmanaged explicit `malloc`
   - libarchive manages memory internally
   - Automatic cleanup on error

### Known Limitations

1. **SSL Certificate Verification: DISABLED**
   - Reason: 3DS has no certificate store
   - Impact: Possible MITM attacks
   - Note: Common limitation in 3DS homebrew apps

2. **File Permissions: IGNORED**
   - Reason: SD card uses FAT32
   - Impact: Permissions not preserved
   - Normal on 3DS

---

## POSIX Stubs

The 3DS does not implement some POSIX functions. Safe stubs have been created:

```c
// Stubs for unavailable functions
mode_t umask(mode_t mask) {
    return 0022;  // Default umask
}

struct passwd *getpwnam(const char *name) {
    return NULL;  // No user database
}

struct group *getgrnam(const char *name) {
    return NULL;  // No group database
}

// LZ4 non supportato
int LZ4_decompress_safe(...) {
    return -1;  // Not supported
}
```

---

## Building

### Prerequisites
```bash
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-citro3d 3ds-citro2d \
                   3ds-libarchive 3ds-zlib 3ds-bzip2 3ds-xz 3ds-zstd
```

### Build
```bash
make clean
make
```

### Output
- `3ds-zip-extractor.3dsx` - Eseguibile per Homebrew Launcher
- `3ds-zip-extractor.elf` - File debug con simboli

### Linked Libraries
```makefile
LIBS := -lcitro2d -lcitro3d -lcurl -lmbedtls -lmbedx509 -lmbedcrypto \
        -larchive -llzma -lbz2 -lzstd -lz -lctru -lm
```

---

## Main API

### Archive Extractor

```c
// Detect archive type
ArchiveType detect_archive_type(const char* file_path);

// Get type name
const char* get_archive_type_name(ArchiveType type);

// Extract archive
int extract_archive_libarchive(
    const char* archive_path,
    const char* output_dir,
    ExtractCallback callback,
    void* user_data
);

// Callback for progress
typedef bool (*ExtractCallback)(
    int file_count,
    const char* current_file,
    void* user_data
);
```

---

## File Structure

```
3ds-zip-extractor/
├── source/
│   ├── main.c              # Main logic
│   ├── archive_extractor.c # Multi-format extraction
│   └── gui.c               # Graphic rendering
├── include/
│   ├── gui.h
│   └── archive_extractor.h
├── docs/
│   ├── TECHNICAL.md        # This file
│   ├── USER_GUIDE.md       # User guide
│   ├── DEVELOPMENT.md      # Developer notes
│   └── archive/            # Historical files
├── build/                  # Compiled files
├── Makefile
├── config.txt              # Config example
├── README.md               # Main documentation
├── CONTRIBUTING.md         # Contribution guides
├── SECURITY.md             # Security policy
└── LICENSE                 # MIT License
```

---

## Debug and Testing

### Debug Build
```bash
make clean
make V=1  # Verbose output
```

### Error Checking
```bash
# Search for memory leaks (if available on 3DS)
# Check console log during execution
```

### Testing on Citra Emulator
```bash
# The app can be tested on Citra 3DS emulator
citra-qt 3ds-zip-extractor.3dsx
```

**Note**: Some network functions may not work on emulator.

---

## Troubleshooting

### Build Errors

**Error: `libarchive not found`**
```bash
sudo dkp-pacman -S 3ds-libarchive
```

**Error: `undefined reference to LZ4_*`**
- Normal, stub implemented
- If persists: verify that `archive_extractor.c` is compiled

### Runtime Errors

**App crashes on startup**
- Verify `config.txt` is present
- Check config file format

**Download fails**
- Verify 3DS WiFi connection
- Test URL in browser first

**Extraction fails**
- Verify space on SD card
- File might be corrupted
- Format might not be supported

---

## Future Optimizations

### Performance
- [ ] Multi-threaded decompression on New 3DS
- [ ] Cache for small files
- [ ] Direct streaming (download + extract)

### Functionality
- [ ] LZ4 support (if library becomes available)
- [ ] Archive content preview before extraction
- [ ] Selective file extraction
- [ ] Archive creation (not just extraction)

### UI/UX
- [ ] Fully GPU-based graphics
- [ ] Customizable themes
- [ ] Touch screen support
- [ ] 3DS file browser integration

---

## References

### Libraries
- [libarchive](https://www.libarchive.org/) - Multi-format archive library
- [libcurl](https://curl.se/libcurl/) - HTTP/HTTPS client
- [mbedTLS](https://www.trustedfirmware.org/projects/mbed-tls/) - SSL/TLS
- [zlib](https://www.zlib.net/) - DEFLATE compression

### 3DS Development
- [devkitPRO](https://devkitpro.org/) - 3DS toolchain
- [libctru](https://github.com/devkitPro/libctru) - 3DS system library
- [citro3d](https://github.com/devkitPro/citro3d) - GPU library

### Documentation
- [3DS Brew Wiki](https://www.3dbrew.org/)
- [libarchive Manual](https://github.com/libarchive/libarchive/wiki)

---

**Document Version**: 1.0  
**Date**: February 2, 2026  
**Author**: Marcogn
