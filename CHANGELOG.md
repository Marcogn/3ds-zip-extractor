# Changelog

## [1.0] - 2026-02-05

### First Stable Release

#### Features
- Graphical interface with citro2d/citro3d (dark blue theme)
- Multi-format archive support via libarchive
- HTTP/HTTPS downloads via libcurl
- Google Drive URL conversion
- Auto-config creation on first run
- Sleep mode support
- LED notifications (green=complete, pink=download done)
- File browser for extraction path
- Download queue system
- Auto-retry for failed downloads

#### Supported Formats
- ZIP, TAR, 7Z, RAR (read-only)
- TAR.GZ, TAR.BZ2, TAR.XZ, TAR.ZSTD
- GZIP, BZIP2, XZ, ZSTD

#### Known Limitations
- Large archives (>200MB) may cause issues on Old 3DS
- Password-protected archives not supported
- LZ4 format not supported

---

Report bugs at: https://github.com/Marcogn/3ds-zip-extractor/issues
