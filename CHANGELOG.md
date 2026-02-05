# Changelog

All notable changes to this project will be documented in this file.

## [2.0.2] - 2026-02-05

### Added
- Binary distribution: .3dsx file now included in repository
- Automatic config file creation on first run with example URLs
- File browser for selecting custom extraction paths
- Download queue with status tracking for multiple files
- Google Drive URL automatic conversion for direct downloads

### Fixed
- Critical stability issues on real hardware (data abort errors)
- Stack overflow with large data structures - moved to heap allocation
- Application icon now displays correctly (48x48 RGBA format)
- Memory leaks in download and extraction routines

### Changed
- GUI temporarily disabled in favor of stable console interface
- Large structures (DownloadQueue, FileBrowser) now heap-allocated
- Improved error messages throughout the application
- Better user guidance during initial setup

### Removed
- Obsolete build scripts and icon generation tools
- Italian and outdated documentation files

## [2.0.1] - 2026-02-02

### Fixed
- Data abort crashes on Old 3DS and New 3DS hardware
- GPU rendering conflicts with citro3d/citro2d libraries

### Changed
- Console-only mode as default for stability
- Heap allocation to prevent stack overflow

## [2.0.0] - 2026-02-02

### Added
- Multi-format archive support (11+ formats)
- Archive types: ZIP, TAR, 7Z, RAR (read-only)
- Compressed TAR: TAR.GZ, TAR.BZ2, TAR.XZ, TAR.ZSTD
- Single compressed files: GZIP, BZIP2, XZ, ZSTD
- Automatic format detection via file signatures (magic bytes)
- libarchive integration for robust extraction
- Download resume capability for interrupted transfers
- Automatic retry for failed downloads (configurable)
- Progress tracking for both download and extraction
- Multi-file queue system

### Changed
- Complete rewrite of extraction engine
- Improved memory management and error handling
- Performance optimizations for New 3DS hardware

## [1.0.0] - 2026-01-15

### Added
- Initial release with basic ZIP extraction
- HTTP/HTTPS download support via libcurl
- Simple console interface
- Configuration file support (config.txt)
- Progress display during operations

---

## Notes

### Supported Formats
The application can extract archives in these formats:
- ZIP (deflate, store)
- TAR (uncompressed)
- TAR.GZ, TAR.BZ2, TAR.XZ, TAR.ZSTD
- 7-Zip (.7z)
- RAR (read-only, no RAR5)
- Single compressed files (GZIP, BZIP2, XZ, ZSTD)

### Known Limitations
- LZ4 format not supported (library unavailable for 3DS)
- Password-protected archives not supported
- Very large archives (>200MB) may have issues on Old 3DS
- GPU rendering disabled in v2.0.2 for stability

### Migration from v1.0
No configuration changes required. The config.txt format is unchanged.
Simply replace the .3dsx file and restart.

