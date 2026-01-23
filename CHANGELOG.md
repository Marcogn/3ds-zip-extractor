# Changelog

All notable changes to the 3DS Zip Extractor project will be documented in this file.

## [1.0.0] - 2026-01-23

### Initial Release

#### Added
- Complete 3DS homebrew application for downloading and extracting archives
- HTTP/HTTPS download support with libcurl
- Download progress tracking with real-time display
- Resume support for interrupted downloads
- Google Drive URL support with automatic conversion to direct download links
  - Supports `/file/d/FILE_ID/view` format
  - Supports `/open?id=FILE_ID` format
- Multiple archive format support via libarchive:
  - ZIP (.zip)
  - TAR (.tar, .tar.gz, .tar.bz2, .tar.xz)
  - 7-Zip (.7z)
  - RAR (.rar)
  - And more formats supported by libarchive
- Extraction progress tracking with file count display
- Large file handling with streaming (no full file buffering required)
- Configurable extract directory (default: `sdmc:/extracted/`)
- User interface with:
  - Download progress (MB downloaded / total MB)
  - Download percentage
  - Extraction file counter
  - Current file being extracted
  - Cancellation support (B button)
- Error handling for:
  - Network initialization failures
  - Download errors
  - Extraction errors
  - File system errors
- Temporary file cleanup after extraction
- SSL/TLS support via mbedtls

#### Documentation
- Comprehensive README with features and usage
- Detailed INSTALL guide with prerequisites and setup
- EXAMPLES file with test URLs and Google Drive instructions
- Build script for automated compilation
- MIT License

#### Build System
- Makefile configured for devkitARM/libctru
- Dependencies on standard 3DS libraries:
  - libctru (3DS system)
  - libcurl (HTTP downloads)
  - libarchive (archive extraction)
  - mbedtls (SSL/TLS)
  - zlib (compression)
- .gitignore for build artifacts

### Technical Details
- Download buffer: 128KB for efficient streaming
- Supports files larger than available RAM
- Resume from byte position on interrupted downloads
- Real-time UI updates during operations
- Console-based interface for compatibility

## Future Enhancements (Potential)

### Planned Features
- [ ] URL input via software keyboard instead of hardcoding
- [ ] Configuration file support for URLs and settings
- [ ] Download queue for multiple files
- [ ] File browser to select extract location
- [ ] Better error messages with troubleshooting hints
- [ ] Download history
- [ ] Bandwidth limiting option
- [ ] Proxy support
- [ ] Authentication support for protected files
- [ ] Archive listing before extraction (preview)
- [ ] Selective file extraction from archives
- [ ] Multiple extract format support (extract to same name, numbered folders, etc.)

### Known Limitations
- URL must be hardcoded in source before compilation
- No GUI, console-based interface only
- Google Drive large files (>100MB) may require additional handling for virus scan page
- Single download/extraction at a time
- No background operation support

## Contributing

Contributions are welcome! Please feel free to submit pull requests for:
- Bug fixes
- New features from the "Future Enhancements" list
- Documentation improvements
- Performance optimizations
- Additional archive format support

## Version History

- **1.0.0** (2026-01-23): Initial release with core functionality
