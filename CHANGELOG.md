# Changelog

All notable changes to the 3DS Zip Extractor project will be documented in this file.

## [1.2.0] - 2026-01-23

### Added
- **Enhanced configuration file**: Now supports both settings and URLs in single file (`config.txt`)
- **Download queue management**: 
  - Visual queue status with indicators ([ ], [>], [✓], [X], [-])
  - Navigate queue with L/R buttons (paginated view)
  - Skip failed downloads with Y button
  - Retry failed downloads after completion with A button
- **Auto-retry feature**: Automatically retry failed downloads
  - Configurable with `auto_retry=true/false` in config
  - Configurable max retries with `max_retries=<0-10>` setting
- **Configurable extract path**: Set custom extract location via `extract_path=` setting
- **Download state tracking**: Each download has status (pending, in progress, completed, failed, skipped)
- **Enhanced error messages**: Per-download error tracking

### Changed
- Configuration file moved from `urls.txt` to `config.txt` (still supports old format)
- Queue view shows more information (status indicators, error messages)
- Summary screen now shows skipped count in addition to successful/failed
- Can retry failed downloads without restarting app
- Settings are now part of config file instead of source code constants

### Improved
- Better download workflow with queue management
- More user control over failed downloads
- Clearer visual feedback with status indicators
- No need to restart app to retry failed downloads

## [1.1.0] - 2026-01-23

### Added
- **Configuration file support**: URLs are now read from `sdmc:/3ds/zip-extractor/urls.txt`
- **Multiple downloads**: Support for downloading and extracting multiple files sequentially
- **URL preview**: Press X to view all configured URLs before starting
- **Batch statistics**: Shows summary with successful/failed counts and total files extracted
- **Comment support**: Lines starting with # in urls.txt are treated as comments
- **Empty line handling**: Empty lines in urls.txt are automatically skipped
- Support for up to 50 URLs in configuration file

### Changed
- URLs are no longer hardcoded in source code
- Configuration is now user-friendly via text file
- UI updated to show "file X of Y" during processing
- Download progress display separated into its own function
- Main loop refactored to support batch processing

### Improved
- Better user experience with configuration file approach
- No need to recompile when changing URLs
- Easier to manage multiple downloads

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
- [ ] Software keyboard for adding/editing URLs directly from the app
- [ ] Edit configuration file from within the app
- [ ] Pause/resume individual downloads
- [ ] Reorder queue items
- [ ] File browser to select extract location
- [ ] Save/load multiple configuration profiles
- [ ] Better error messages with troubleshooting hints
- [ ] Download history and statistics
- [ ] Bandwidth limiting option
- [ ] Proxy support
- [ ] Authentication support for protected files
- [ ] Archive listing before extraction (preview contents)
- [ ] Selective file extraction from archives
- [ ] Multiple extract format support (extract to same name, numbered folders, etc.)
- [ ] Parallel downloads (multiple files at once)
- [ ] Download scheduling

### Known Limitations
- Sequential processing only (one file at a time)
- Maximum 50 URLs in configuration file
- No GUI, console-based interface only
- Google Drive large files (>100MB) may require additional handling for virus scan page
- No background operation support
- Cannot pause/resume individual downloads (only cancel all)

## Contributing

Contributions are welcome! Please feel free to submit pull requests for:
- Bug fixes
- New features from the "Future Enhancements" list
- Documentation improvements
- Performance optimizations
- Additional archive format support

## Version History

- **1.2.0** (2026-01-23): Added enhanced configuration and queue management
- **1.1.0** (2026-01-23): Added configuration file support and multiple downloads
- **1.0.0** (2026-01-23): Initial release with core functionality
