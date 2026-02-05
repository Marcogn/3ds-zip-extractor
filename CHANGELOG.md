# Changelog

All notable changes to this project will be documented in this file.

## [0.1.0-prealpha] - 2026-02-05

### Added
- Graphical interface with citro2d/citro3d (dark blue theme)
- Multi-format archive support via libarchive
- HTTP/HTTPS downloads via libcurl
- Google Drive URL conversion
- Auto-config creation on first run
- Sleep mode support
- LED notifications (green=complete, pink=download done)
- File browser for extraction path
- Download queue system

### Supported Formats
- ZIP, TAR, 7Z, RAR (read-only)
- TAR.GZ, TAR.BZ2, TAR.XZ, TAR.ZSTD
- GZIP, BZIP2, XZ, ZSTD

### Known Issues
- Pre-alpha: expect bugs
- Large archives (>200MB) may cause issues on Old 3DS
- Password-protected archives not supported
- LZ4 not supported

---

## Notes

This is a pre-alpha release. The application is still under development.
Report bugs at: https://github.com/Marcogn/3ds-zip-extractor/issues
