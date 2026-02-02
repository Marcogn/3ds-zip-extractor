# 3DS Zip Extractor
<p align="center">
  <img src="icon.png" alt="Icon" width="96">
</p>
<p align="center">
  <strong>Download files directly on your New Nintendo 3DS</strong>
</p>
---
> **✅ BUILD WORKING!** The project now compiles successfully.
> 
> **Fully functional features:**
> - ✅ HTTP/HTTPS downloads with resume
> - ✅ Google Drive URL conversion
> - ✅ Queue management and GUI
> - ✅ **ZIP extraction working!** Supports deflate and store compression
---
## Quick Start
### Build
```bash
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-citro3d 3ds-citro2d
git clone https://github.com/Marcogn/3ds-zip-extractor.git
cd 3ds-zip-extractor
make
```
### Install
Copy to SD card:
- `3ds-zip-extractor.3dsx` → `/3ds/`
- `config.txt` → `/3ds/zip-extractor/`
### Configure
Edit `/3ds/zip-extractor/config.txt`:
```
extract_path=sdmc:/downloads/
auto_retry=true
max_retries=3
# URLs
https://example.com/file.zip
https://drive.google.com/file/d/FILE_ID/view
```
### Run
Launch from Homebrew Launcher, press A to start downloads.
---
## Features
✅ **HTTP/HTTPS downloads** with resume
✅ **Google Drive** support (auto virus scan bypass for >100MB)
✅ **ZIP extraction** (deflate and store compression)
✅ **Queue management** (skip, retry, status)
✅ **Hybrid GUI** (console + graphics)
✅ **File browser** for destination
---
## Controls
- **A** - Start downloads
- **X** - View queue
- **SELECT** - Browse path
- **B** - Cancel
- **START** - Exit
---
## Google Drive
Automatically converts sharing links:
```
✅ drive.google.com/file/d/FILE_ID/view
✅ drive.google.com/open?id=FILE_ID
```
Share with "Anyone with the link", paste URL in config.txt.
---
## Documentation
- **[USER_GUIDE.md](USER_GUIDE.md)** - Complete manual
- **[DEVELOPMENT.md](DEVELOPMENT.md)** - Technical details
---
## Current Limitation
Archives are **downloaded but not extracted**. 
Workaround: Extract on PC for now.
ZIP extraction coming in next update.
---
## License
MIT - See [LICENSE](LICENSE)
---
<p align="center">
  <sub>v0.1-MVP | 2026-02-02</sub>
</p>
