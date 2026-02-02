# Google Drive & 7zip Support Guide

## 🇮🇹 Italiano

### Supporto Google Drive

Questa applicazione supporta il download diretto da Google Drive con conversione automatica degli URL.

#### Formati URL Supportati

1. **Formato Condivisione Standard**
   ```
   https://drive.google.com/file/d/1ABC...XYZ/view
   ```
   Viene convertito in:
   ```
   https://drive.google.com/uc?export=download&id=1ABC...XYZ&confirm=t
   ```

2. **Formato "Apri con"**
   ```
   https://drive.google.com/open?id=1ABC...XYZ
   ```
   Viene convertito automaticamente.

3. **Formato Download Diretto**
   ```
   https://drive.google.com/uc?id=1ABC...XYZ
   ```
   Già ottimizzato, viene aggiunto solo `confirm=t`.

#### File Grandi (>100MB)

Google Drive mostra una pagina di avviso per la scansione antivirus sui file >100MB. L'applicazione:
- **Aggiunge automaticamente** il parametro `confirm=t`
- **Bypassa** la pagina di conferma
- **Scarica direttamente** il file

#### Come Ottenere il Link

1. Vai su Google Drive
2. Fai clic destro sul file
3. Seleziona "Ottieni link"
4. Assicurati che sia impostato su "Chiunque abbia il link"
5. Copia l'URL completo
6. Incolla nel `config.txt`

#### Limitazioni

- ❌ **Cartelle non supportate** - Solo file singoli
- ✓ File devono essere pubblici o con "Chiunque abbia il link"
- ✓ Nessun limite di dimensione teorico (dipende dalla connessione)
- ✓ Supporta file ZIP, 7Z, TAR, RAR, ecc.

#### Esempio Completo

**config.txt:**
```
# Impostazioni
extract_path=sdmc:/giochi/
auto_retry=true
max_retries=3

# Download da Google Drive
https://drive.google.com/file/d/1ABC123DEF456/view
https://drive.google.com/open?id=1XYZ789GHI012
```

### Supporto 7zip

L'applicazione supporta completamente il formato 7-Zip (.7z) tramite libarchive.

#### Algoritmi di Compressione Supportati

- **LZMA**: Compressione standard 7z (migliore ratio)
- **LZMA2**: Versione migliorata (più veloce)
- **PPMD**: Ottimo per file di testo
- **BZIP2**: Alternativa a LZMA
- **Deflate**: Compatibilità con ZIP

#### Caratteristiche

✓ **Estrazione Completa**
- File singoli e multipli
- Struttura directory preservata
- Permessi file (dove possibile)

✓ **Compressione Solida**
- Supporto per archivi solidi
- Streaming efficiente
- Basso uso di memoria

✓ **Encryption**
- ⚠️ Password protetti NON supportati
- Solo archivi non crittografati

#### Formati Aggiuntivi

Oltre a 7Z, sono supportati:

| Formato | Estensione | Compressione | Note |
|---------|------------|--------------|------|
| ZIP | .zip | Varie | Completo |
| 7-Zip | .7z | LZMA/LZMA2 | Completo |
| TAR | .tar | Nessuna | Completo |
| TAR.GZ | .tar.gz | GZIP | Completo |
| TAR.BZ2 | .tar.bz2 | BZIP2 | Completo |
| TAR.XZ | .tar.xz | LZMA | Completo |
| RAR | .rar | Proprietario | Solo lettura |
| GZIP | .gz | GZIP | File singolo |
| BZIP2 | .bz2 | BZIP2 | File singolo |

#### Esempio d'Uso

**config.txt:**
```
extract_path=sdmc:/homebrew/

# Archivi 7zip
https://example.com/game.7z
https://example.com/tool.zip
https://example.com/data.tar.xz

# Da Google Drive
https://drive.google.com/file/d/FILE_ID/view
```

#### Performance

- **ZIP**: Molto veloce ✓✓✓
- **7Z (LZMA)**: Lento ma ottimo ratio ✓
- **TAR.GZ**: Veloce ✓✓
- **TAR.XZ**: Lento ✓

---

## 🇬🇧 English

### Google Drive Support

This application supports direct download from Google Drive with automatic URL conversion.

#### Supported URL Formats

1. **Standard Sharing Format**
   ```
   https://drive.google.com/file/d/1ABC...XYZ/view
   ```
   Converted to:
   ```
   https://drive.google.com/uc?export=download&id=1ABC...XYZ&confirm=t
   ```

2. **"Open with" Format**
   ```
   https://drive.google.com/open?id=1ABC...XYZ
   ```
   Automatically converted.

3. **Direct Download Format**
   ```
   https://drive.google.com/uc?id=1ABC...XYZ
   ```
   Already optimized, only `confirm=t` is added.

#### Large Files (>100MB)

Google Drive shows a virus scan warning page for files >100MB. The application:
- **Automatically adds** the `confirm=t` parameter
- **Bypasses** the confirmation page
- **Downloads directly** the file

#### How to Get the Link

1. Go to Google Drive
2. Right-click on the file
3. Select "Get link"
4. Make sure it's set to "Anyone with the link"
5. Copy the full URL
6. Paste into `config.txt`

#### Limitations

- ❌ **Folders not supported** - Only single files
- ✓ Files must be public or with "Anyone with the link"
- ✓ No theoretical size limit (depends on connection)
- ✓ Supports ZIP, 7Z, TAR, RAR, etc. files

#### Complete Example

**config.txt:**
```
# Settings
extract_path=sdmc:/games/
auto_retry=true
max_retries=3

# Download from Google Drive
https://drive.google.com/file/d/1ABC123DEF456/view
https://drive.google.com/open?id=1XYZ789GHI012
```

### 7zip Support

The application fully supports the 7-Zip format (.7z) through libarchive.

#### Supported Compression Algorithms

- **LZMA**: Standard 7z compression (best ratio)
- **LZMA2**: Improved version (faster)
- **PPMD**: Great for text files
- **BZIP2**: Alternative to LZMA
- **Deflate**: ZIP compatibility

#### Features

✓ **Complete Extraction**
- Single and multiple files
- Directory structure preserved
- File permissions (where possible)

✓ **Solid Compression**
- Support for solid archives
- Efficient streaming
- Low memory usage

✓ **Encryption**
- ⚠️ Password protected NOT supported
- Only unencrypted archives

#### Additional Formats

Besides 7Z, the following are supported:

| Format | Extension | Compression | Notes |
|--------|-----------|-------------|-------|
| ZIP | .zip | Various | Complete |
| 7-Zip | .7z | LZMA/LZMA2 | Complete |
| TAR | .tar | None | Complete |
| TAR.GZ | .tar.gz | GZIP | Complete |
| TAR.BZ2 | .tar.bz2 | BZIP2 | Complete |
| TAR.XZ | .tar.xz | LZMA | Complete |
| RAR | .rar | Proprietary | Read-only |
| GZIP | .gz | GZIP | Single file |
| BZIP2 | .bz2 | BZIP2 | Single file |

#### Usage Example

**config.txt:**
```
extract_path=sdmc:/homebrew/

# 7zip archives
https://example.com/game.7z
https://example.com/tool.zip
https://example.com/data.tar.xz

# From Google Drive
https://drive.google.com/file/d/FILE_ID/view
```

#### Performance

- **ZIP**: Very fast ✓✓✓
- **7Z (LZMA)**: Slow but best ratio ✓
- **TAR.GZ**: Fast ✓✓
- **TAR.XZ**: Slow ✓

---

## Troubleshooting

### Google Drive Issues

**Problem**: "Download failed" with Google Drive URL
- ✓ Check the file is public or "Anyone with the link"
- ✓ Try copying the link again
- ✓ Make sure it's a file link, not a folder

**Problem**: Download stops at virus scan page
- ✓ The app should automatically bypass this
- ✓ If not, the URL conversion might have failed
- ✓ Check the console for conversion messages

**Problem**: "File ID not found"
- ✓ URL format might be unsupported
- ✓ Try using the standard sharing link format
- ✓ Check for extra characters in the URL

### 7zip Issues

**Problem**: "Failed to open archive"
- ✓ File might be corrupted
- ✓ Download might have failed
- ✓ Check if the file is actually a .7z archive

**Problem**: "Extraction failed" with 7z
- ✓ Archive might be password protected (not supported)
- ✓ Check free space on SD card
- ✓ Some very large 7z files might cause memory issues

**Problem**: Very slow extraction
- ✓ 7z with LZMA compression is inherently slow
- ✓ This is normal for highly compressed archives
- ✓ Consider using ZIP for faster extraction

---

## Advanced Tips

### Optimal Archive Format for 3DS

1. **For Speed**: Use ZIP with deflate compression
2. **For Size**: Use 7Z with LZMA2
3. **For Compatibility**: Use TAR.GZ

### Google Drive Best Practices

1. Always use "Anyone with the link" permission
2. Don't share folder links, only file links
3. Test the link in a browser first
4. For very large files (>2GB), expect long download times

### Multiple Downloads

You can mix different sources and formats:
```
# config.txt
https://example.com/file1.zip
https://drive.google.com/file/d/ABC123/view
https://cdn.site.com/data.7z
https://mirror.org/archive.tar.xz
```

The app will process them sequentially and show a summary at the end.
