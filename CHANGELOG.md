# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.2] - 2026-02-05

### Added
- Auto-config file creation on first run
- Example configuration file with instructions
- Improved user guidance for initial setup
- File browser for extraction path selection
- Queue management with status display
- Google Drive URL automatic conversion

### Fixed
- Data abort crashes on real hardware
- Stack overflow issues with large structures
- Icon display (proper 48x48 RGBA format)
- Memory allocation moved to heap for stability
- GUI disabled temporarily for maximum stability

### Changed
- Console-only interface for reliability
- Improved error messages and user feedback
- Heap allocation for `DownloadQueue` and `FileBrowser`
- Enhanced stability on Old 3DS hardware

## [2.0.1] - 2026-02-02

### Fixed
- Critical data abort crashes on real 3DS hardware
- Stack overflow with large data structures
- GPU rendering conflicts (citro3d/citro2d)

### Changed
- GUI temporarily disabled for stability
- Console-only mode as default

## [2.0.0] - 2026-02-02

### Added
- Multi-format archive support (11+ formats)
- ZIP, TAR, 7Z, RAR (read-only) extraction
- Compressed TAR variants (GZ, BZ2, XZ, ZSTD)
- Single compressed files (GZIP, BZIP2, XZ, ZSTD)
- Automatic format detection via magic bytes
- libarchive integration for robust extraction
- Download resume support
- Auto-retry for failed downloads
- Progress tracking for downloads and extraction
- Queue system for multiple files

### Changed
- Complete rewrite of extraction engine
- Improved memory management
- Enhanced error handling
- Performance optimizations for New 3DS

### Removed
- Basic ZIP-only extraction (replaced with multi-format)

## [1.0.0] - 2026-01-15

### Added
- Initial release
- Basic ZIP file extraction
- HTTP/HTTPS download support
- Simple console GUI
- Configuration file support
- Progress display

---

## Version Compatibility

- **v2.0.x**: Requires devkitPRO with libarchive, libcurl, compression libraries
- **v1.0.x**: Basic dependencies only (zlib, libcurl)

## Migration Guide

### From v1.0 to v2.0

No configuration changes required. The `config.txt` format remains the same.

**Benefits of upgrading:**
- Support for 10+ additional archive formats
- Automatic format detection
- Improved stability and error handling
- Better performance on New 3DS
- Auto-config creation feature

**Known Issues:**
- LZ4 format not supported (library unavailable on 3DS)
- Password-protected archives not supported
- GPU rendering disabled in v2.0.2 for stability

---

## Support

For issues, feature requests, or contributions, visit:
https://github.com/Marcogn/3ds-zip-extractor

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
