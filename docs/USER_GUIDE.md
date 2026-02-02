# 3DS Zip Extractor - Complete User Guide

## 📚 Table of Contents

- [Quick Start](#quick-start)
- [Google Drive Downloads](#google-drive-downloads)
- [Supported Formats](#supported-formats)
- [Configuration Examples](#configuration-examples)
- [File Browser](#file-browser)
- [Queue Management](#queue-management)
- [Troubleshooting](#troubleshooting)

---

## Quick Start

### Installation

1. **On PC:** Compile the project (see main README.md)
2. **Copy to SD card:**
   ```
   /3ds/3ds-zip-extractor.3dsx
   /3ds/zip-extractor/config.txt
   ```
3. **On 3DS:** Launch from Homebrew Launcher

### Basic config.txt

Create `/3ds/zip-extractor/config.txt`:

```
# Settings
extract_path=sdmc:/extracted/
auto_retry=true
max_retries=3

# URLs (one per line)
https://example.com/file.zip
https://example.com/data.7z
```

### Controls

**Main Menu:**
- **A** - Start downloads
- **X** - View queue status
- **SELECT** - Open file browser
- **START** - Exit

**File Browser:**
- **D-Pad ↑↓** - Navigate
- **A** - Enter folder
- **Y** - Use current folder
- **B** - Back

**Queue View:**
- **L/R** - Change page
- **Y** - Skip failed downloads
- **A** - Retry/Continue
- **B** - Back to menu

---

## Google Drive Downloads

### Supported URL Formats

The app automatically converts Google Drive links:

```
✅ https://drive.google.com/file/d/FILE_ID/view
✅ https://drive.google.com/file/d/FILE_ID/edit
✅ https://drive.google.com/open?id=FILE_ID
✅ https://drive.google.com/uc?id=FILE_ID
```

### How to Get the Link

1. Open file in Google Drive
2. Click "Share" → "Get link"
3. Set to "Anyone with the link"
4. Copy the URL
5. Paste into config.txt

### Large Files (>100MB)

Google Drive shows a virus scan warning for files >100MB. The app automatically bypasses this by adding `confirm=t` to the URL.

**Before:**
```
https://drive.google.com/file/d/1ABC123/view
```

**After (automatic):**
```
https://drive.google.com/uc?export=download&id=1ABC123&confirm=t
```

### Limitations

- ❌ **Folder links NOT supported** - Only single files
- ✅ Files must be public or "Anyone with the link"
- ✅ No size limit (depends on connection speed)
- ⚠️ Very large files (>2GB) may timeout

---

## Supported Formats

### Archive Formats

| Format | Extension | Compression | Status |
|--------|-----------|-------------|--------|
| ZIP | .zip | Various | ✅ Full support |
| 7-Zip | .7z | LZMA/LZMA2 | ✅ Full support* |
| TAR | .tar | None | ✅ Full support |
| TAR.GZ | .tar.gz | GZIP | ✅ Full support |
| TAR.BZ2 | .tar.bz2 | BZIP2 | ⚠️ Limited* |
| TAR.XZ | .tar.xz | LZMA | ⚠️ Limited* |
| RAR | .rar | Proprietary | ✅ Read-only |
| GZIP | .gz | GZIP | ✅ Single file |
| BZIP2 | .bz2 | BZIP2 | ⚠️ Single file* |

**\* Note:** Advanced compression formats (7z, xz, bz2) require additional libraries that may not be available due to toolchain limitations. See main README for details.

### ZIP Compression Methods

ZIP files support multiple compression methods:
- **Store** (no compression) - Fastest extraction
- **Deflate** (standard) - Fast, good compression
- **BZIP2** - Better compression, slower*
- **LZMA** - Best compression, slowest*
- **PPMD** - Good for text files*

### Performance on 3DS Hardware

Expected extraction times (100MB file):

| Format | Time | Speed |
|--------|------|-------|
| ZIP (deflate) | 15-30 sec | ⚡⚡⚡ Fast |
| ZIP (store) | 5-10 sec | ⚡⚡⚡ Very fast |
| TAR.GZ | 30-60 sec | ⚡⚡ Medium |
| 7Z (LZMA) | 60-180 sec | ⚡ Slow |
| TAR.XZ | 120-300 sec | 🐌 Very slow |

**Recommendation:** Use ZIP with deflate compression for best balance between size and speed on 3DS.

---

## Configuration Examples

### Example 1: Basic Download

```
# Simple configuration
extract_path=sdmc:/games/

# Direct URLs
https://example.com/game.zip
https://example.com/update.zip
```

### Example 2: Multiple Sources

```
# Mix different sources
extract_path=sdmc:/homebrew/
auto_retry=true
max_retries=3

# Direct download
https://example.com/tool1.zip

# Google Drive
https://drive.google.com/file/d/1ABC123/view

# Another direct URL
https://cdn.site.com/data.7z
```

### Example 3: Advanced Settings

```
# Full configuration with all options
extract_path=sdmc:/downloaded/
auto_retry=true
max_retries=5

# Comments are allowed
# Multiple archives in sequence
https://example.com/part1.zip
https://example.com/part2.zip
https://example.com/part3.zip

# Google Drive files
https://drive.google.com/open?id=FILE_ID_HERE
```

### Example 4: Test Downloads

Small test files for verifying functionality:

```
extract_path=sdmc:/test/

# Small test files (fast download)
https://speedtest.tele2.net/1MB.zip
https://speedtest.tele2.net/5MB.zip
https://speedtest.tele2.net/10MB.zip
```

---

## File Browser

### Using the File Browser

Press **SELECT** in the main menu to open the file browser.

```
┌─────────────────────────────────────────┐
│ File Browser - Select Extract Path      │
│ ======================================== │
│ Current: sdmc:/games/                    │
│ ======================================== │
│                                          │
│   > ../                                  │
│     3ds/                                 │
│     extracted/                           │
│     games/                               │
│     homebrew/                            │
│     Nintendo 3DS/                        │
│                                          │
│ D-Pad: Navigate  A: Select/Enter         │
│ Y: Use Current  B: Cancel                │
└─────────────────────────────────────────┘
```

### Navigation

- **Folders** end with `/`
- **Selected item** has `>` marker
- **../** goes to parent directory
- **Current path** shown at top

### Creating Folders

Currently not supported in app. Create folders on PC before use, or use existing folders.

---

## Queue Management

### Queue Status Indicators

```
[ ] Pending      - Not yet started
[>] In Progress  - Currently downloading
[✓] Completed    - Successfully extracted
[X] Failed       - Error occurred
[-] Skipped      - Manually skipped
```

### Viewing Queue (Press X)

```
┌─────────────────────────────────────────┐
│ Zip Extractor - Queue Status            │
│ ======================================== │
│                                          │
│ Showing 1-5 of 8                         │
│                                          │
│ [✓] https://example.com/file1.zip        │
│ [✓] https://example.com/file2.zip        │
│ [>] https://example.com/file3.zip        │
│ [ ] https://example.com/file4.zip        │
│ [ ] https://example.com/file5.zip        │
│                                          │
│ Page 1/2                                 │
│                                          │
│ L/R: Change page  Y: Skip failed         │
│ A: Continue  B: Back  START: Exit        │
└─────────────────────────────────────────┘
```

### Managing Failed Downloads

If downloads fail:
1. Press **A** after all complete to retry failed items
2. Or press **Y** in queue view to skip all failed
3. Failed items show error message in console

### Auto-Retry Feature

Enable in config.txt:
```
auto_retry=true
max_retries=3
```

The app will automatically retry failed downloads up to the specified number of times.

---

## Troubleshooting

### Download Issues

**Problem:** Download fails with "Connection refused"
- ✓ Check your internet connection
- ✓ Verify the URL is accessible in a browser
- ✓ Some sites may block direct downloads

**Problem:** "Failed to resolve host"
- ✓ Check DNS settings on 3DS
- ✓ Try rebooting the 3DS
- ✓ Verify WiFi connection is active

**Problem:** Download starts but stops at 0%
- ✓ The file may be too large
- ✓ Server might not support resume
- ✓ Check free space on SD card

### Google Drive Issues

**Problem:** "Download failed" with Google Drive URL
- ✓ Ensure file is set to "Anyone with the link"
- ✓ Try copying the link again (avoid shortened links)
- ✓ Make sure it's a file link, not a folder

**Problem:** Download redirects to virus scan page
- ✓ The app should handle this automatically
- ✓ If not, the URL conversion might have failed
- ✓ Try using the `/file/d/FILE_ID/view` format

**Problem:** "File too large" error
- ✓ Some files >5GB might have issues
- ✓ Try downloading in parts if possible
- ✓ Check SD card has enough free space

### Extraction Issues

**Problem:** "Failed to open archive"
- ✓ File might be corrupted (check download completed)
- ✓ Format might not be supported
- ✓ Check if file is actually an archive (not HTML error page)

**Problem:** "Extraction failed" midway
- ✓ SD card might be full
- ✓ File might be corrupted
- ✓ Archive might be password protected (not supported)

**Problem:** Very slow extraction
- ✓ 7z/xz formats are inherently slow on 3DS
- ✓ This is normal for highly compressed files
- ✓ Consider using ZIP format for faster extraction

### Configuration Issues

**Problem:** "Config file not found"
- ✓ Create `/3ds/zip-extractor/config.txt` on SD card
- ✓ Check the exact path and filename
- ✓ Ensure file has Unix line endings (LF, not CRLF)

**Problem:** "No URLs found"
- ✓ Check that URLs are not commented out (#)
- ✓ Verify one URL per line
- ✓ Ensure no empty lines between settings and URLs

**Problem:** Settings not applied
- ✓ Settings must be before URLs in config.txt
- ✓ Format: `setting=value` (no spaces around =)
- ✓ Check spelling of setting names

### Performance Issues

**Problem:** App crashes during download
- ✓ File might be too large for available RAM
- ✓ Try closing other apps
- ✓ Reboot 3DS and try again

**Problem:** SD card write errors
- ✓ SD card might be failing
- ✓ Try a different SD card
- ✓ Check SD card for errors on PC

---

## Tips & Best Practices

### For Best Performance

1. **Use ZIP format** when possible (fastest extraction)
2. **Close other apps** before starting large downloads
3. **Stable WiFi** - position 3DS near router
4. **Free space** - ensure at least 2x file size free on SD

### For Google Drive

1. **Always use "Anyone with the link"** permission
2. **Avoid shortened links** - use full Google Drive URLs
3. **Test in browser first** to verify file is accessible
4. **For large files (>1GB)** - expect long download times (30+ min)

### For Multiple Downloads

1. **Order matters** - list URLs in desired download order
2. **Enable auto-retry** for unreliable connections
3. **Monitor first download** to ensure setup is correct
4. **Use queue view** (X button) to track progress

### Configuration Tips

1. **Use comments** (#) to organize config.txt
2. **Test with small files** first
3. **Backup working config.txt**
4. **One URL per line** - no exceptions

---

## Need More Help?

- **Main README:** General overview and building instructions
- **DEVELOPMENT.md:** Technical details and compilation issues
- **GitHub Issues:** Report bugs or request features

---

*Last updated: 2026-02-02*
*Part of 3DS Zip Extractor project*
