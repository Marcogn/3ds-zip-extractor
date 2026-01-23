# Example URLs and Configuration

## Creating Your urls.txt File

Create a file at `sdmc:/3ds/zip-extractor/urls.txt` on your SD card with your URLs.

### Basic Example

```
# Simple URL list
https://speedtest.tele2.net/1MB.zip
https://speedtest.tele2.net/5MB.zip
```

### Multiple Downloads Example

```
# Download multiple archives
https://example.com/game-data.zip
https://example.com/game-assets.tar.gz
https://example.com/game-music.7z
```

### Mixed Sources Example

```
# Mix of direct URLs and Google Drive
https://example.com/file1.zip
https://drive.google.com/file/d/1ABC_FILE_ID_XYZ/view
https://example.com/file2.tar.gz
https://drive.google.com/open?id=1DEF_FILE_ID_123
```

## Direct Download URLs

### Small Test Files (< 10MB)
```
https://speedtest.tele2.net/1MB.zip
https://speedtest.tele2.net/5MB.zip
https://speedtest.tele2.net/10MB.zip
```

### Archive Formats
```
# ZIP file
https://github.com/example/archive/refs/heads/main.zip

# TAR.GZ file
https://github.com/example/archive/refs/tags/v1.0.0.tar.gz
```

## Google Drive Examples

### Format 1: File View Link
```
https://drive.google.com/file/d/1ABC_EXAMPLE_FILE_ID_XYZ/view
```
This will be converted to:
```
https://drive.google.com/uc?export=download&id=1ABC_EXAMPLE_FILE_ID_XYZ
```

### Format 2: Open Link
```
https://drive.google.com/open?id=1ABC_EXAMPLE_FILE_ID_XYZ
```
This will be converted to:
```
https://drive.google.com/uc?export=download&id=1ABC_EXAMPLE_FILE_ID_XYZ
```

## How to Get Google Drive File ID

1. Upload your file to Google Drive
2. Right-click the file and select "Get link"
3. Make sure link sharing is set to "Anyone with the link"
4. Copy the link - it will be in one of these formats:
   - `https://drive.google.com/file/d/FILE_ID/view?usp=sharing`
   - `https://drive.google.com/open?id=FILE_ID`
5. Extract the FILE_ID from the URL

Example:
- Full URL: `https://drive.google.com/file/d/1a2B3c4D5e6F7g8H9i0J/view?usp=sharing`
- File ID: `1a2B3c4D5e6F7g8H9i0J`

## Configuring Your SD Card

1. **Create the directory structure:**
```
SD Card
└── 3ds/
    ├── 3ds-zip-extractor.3dsx
    └── zip-extractor/
        └── urls.txt
```

2. **Edit urls.txt with your URLs:**
```
# My downloads
https://speedtest.tele2.net/1MB.zip
https://speedtest.tele2.net/5MB.zip
```

3. **Launch the app** and press A to start all downloads.

## Testing Your Setup

1. **Start with a small file:**
Create `urls.txt` with:
```
https://speedtest.tele2.net/1MB.zip
```

2. **Test multiple downloads:**
Update `urls.txt` with:
```
https://speedtest.tele2.net/1MB.zip
https://speedtest.tele2.net/5MB.zip
```

3. **Finally, use your target files**

## Notes

- Each URL is processed sequentially (one at a time)
- The app will show progress for each download
- At the end, a summary shows successful vs failed downloads
- For Google Drive files larger than 100MB, you may encounter a virus scan warning. The application will attempt to handle this automatically.
- Make sure the file is publicly accessible if using Google Drive
- Test your internet connection on the 3DS before attempting large downloads
- The application will create the extract directory automatically if it doesn't exist
- You can add up to 50 URLs in the configuration file
